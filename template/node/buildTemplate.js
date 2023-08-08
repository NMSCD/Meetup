const fs = require('fs');
const util = require('util');
const sizeOf = require('image-size');
const Handlebars = require('handlebars');
const readFile = util.promisify(fs.readFile);

const publicFolderPath = './public';
const eventMediaMinWidth = 250;
const maxNumMediaOnHomePage = 10;

async function buildTemplates() {
    process.env['NODE_ENV'] = require('../../package.json').version;
    Handlebars.registerHelper('date', require('../handlebar/helpers/date.helper.js'));
    Handlebars.registerHelper('year', require('../handlebar/helpers/year.helper.js'));
    Handlebars.registerHelper('loud', require('../handlebar/helpers/loud.helper.js'));
    Handlebars.registerHelper('urlref', require('../handlebar/helpers/urlref.helper.js'));
    Handlebars.registerHelper('version', require('../handlebar/helpers/version.helper.js'));
    Handlebars.registerHelper('urlrefescaped', require('../handlebar/helpers/urlrefescaped.helper.js'));

    Handlebars.registerPartial('components/documentHead', require('../handlebar/components/documentHead.hbs'));
    Handlebars.registerPartial('components/schemaOrg', require('../handlebar/components/schemaOrg.hbs'));
    Handlebars.registerPartial('components/header', require('../handlebar/components/header.hbs'));
    Handlebars.registerPartial('components/eventMedia', require('../handlebar/components/eventMedia.hbs'));
    Handlebars.registerPartial('components/eventDetails', require('../handlebar/components/eventDetails.hbs'));
    Handlebars.registerPartial('components/footer', require('../handlebar/components/footer.hbs'));
    Handlebars.registerPartial('components/scripts', require('../handlebar/components/scripts.hbs'));

    const siteDataContents = await readFile('./template/data/site.json', 'utf8');
    const orgDataContents = await readFile('./template/data/organisers.json', 'utf8');
    const eventMediaContents = await readFile('./template/data/eventMedia.json', 'utf8');

    const siteData = JSON.parse(siteDataContents);
    const orgData = JSON.parse(orgDataContents);
    const eventMedia = JSON.parse(eventMediaContents);

    const humansArray = [];
    for (const humanKey in siteData.humans) {
        if (Object.hasOwnProperty.call(siteData.humans, humanKey)) {
            humansArray.push(siteData.humans[humanKey]);
        }
    }

    const eventMediaWithSizes = [];
    for (const eventItem of eventMedia) {
        const eventMediaImagesWithSizes = [];
        for (const eventMediaItem of eventItem.images) {
            const dimensions = sizeOf(`./public/assets/img/event/${eventMediaItem.id}.${eventMediaItem.ext}`);
            eventMediaImagesWithSizes.push({
                ...eventMediaItem,
                minWidth: eventMediaMinWidth,
                minHeight: (dimensions.height / dimensions.width) * eventMediaMinWidth,
            });
        }
        eventMediaWithSizes.push({
            ...eventItem,
            images: eventMediaImagesWithSizes,
        })
    }

    const orgDataArray = [];
    for (const orgKey in orgData) {
        if (Object.hasOwnProperty.call(orgData, orgKey)) {
            orgDataArray.push(orgData[orgKey]);
        }
    }

    const enableAnalytics = process?.env?.ENV_ENABLE_ANALYTICS ?? false;
    const enableServiceWorker = process?.env?.ENV_ENABLE_SERVICEWORKER ?? false; // To handle the damn serviceWorker cache on local

    const injectedScriptInHead = `<script>var siteData = ${JSON.stringify({
        nextEventDate: siteData.event.current.date,
        enableAnalytics: enableAnalytics === 'true',
        enableServiceWorker: enableServiceWorker === 'true',
    })}</script>`;

    const projectData = {
        ...siteData,
        eventMedia: eventMediaWithSizes,
        organisers: orgData,
        organisersArray: orgDataArray,
        humansArray: humansArray,
        injectedScriptInHead: injectedScriptInHead,
    };

    const files = [
        { src: 'index.html.hbs', dest: 'index.html' },
        { src: 'terms_and_conditions.html.hbs', dest: 'terms_and_conditions.html' },
        { src: 'privacy_policy.html.hbs', dest: 'privacy_policy.html' },
        { src: '404.html.hbs', dest: '404.html' },

        { src: 'CNAME.hbs', dest: 'CNAME' },
        { src: 'htaccess.hbs', dest: '.htaccess' },
        { src: 'humans.txt.hbs', dest: 'humans.txt' },
        { src: 'robots.txt.hbs', dest: 'robots.txt' },
        { src: 'opensearch.xml.hbs', dest: 'opensearch.xml' },
        { src: 'site.webmanifest.hbs', dest: 'manifest.json' },
        { src: 'site.webmanifest.hbs', dest: 'site.webmanifest' },
        { src: 'sitemap.xml.hbs', dest: 'sitemap.xml' },
    ];

    if (enableServiceWorker === true) {
        files.push({ src: 'serviceWorker.js.hbs', dest: 'serviceWorker.js' });
    }

    const generatedGitIgnoreFiles = [];

    let previousEventMedia = projectData.eventMedia.find(evm => evm.year === (projectData.event.current.year - 1).toString());
    if (previousEventMedia != null) {
        previousEventMedia = {
            ...previousEventMedia,
            images: previousEventMedia.images.slice(0, maxNumMediaOnHomePage),
        }
    }
    const generalTemplateData = {
        ...projectData,
        previousEventMedia: previousEventMedia,
    };

    for (const fileObj of files) {
        const template = await readFile(`./template/handlebar/${fileObj.src}`, 'utf8');
        const templateFunc = Handlebars.compile(template);

        const compiledTemplate = templateFunc(generalTemplateData);
        const destFile = `${publicFolderPath}/${fileObj.dest}`;
        generatedGitIgnoreFiles.push(fileObj.dest);
        fs.writeFile(destFile, compiledTemplate, ['utf8'], () => { });
    }

    const events = [
        projectData.event.current,
        ...projectData.event.previous
    ];
    for (const meetupYear of events) {
        const template = await readFile('./template/handlebar/meetupYear.html.hbs', 'utf8');
        const templateFunc = Handlebars.compile(template);

        const eventMedia = projectData.eventMedia.find(evm => evm.year === meetupYear.year.toString());
        const templateData = {
            ...projectData,
            meetup: meetupYear,
            pageTitle: `NMS ${meetupYear.year} Meetup`,
            meta: {
                ...projectData.meta,
                title: projectData.meta.title.replaceAll(' - ', ` ${meetupYear.year} - `),
            },
            eventMedia: eventMedia,
            hasEventMedia: eventMedia != null,
        };
        const htmlFile = `${meetupYear.year}.html`;
        const compiledTemplate = templateFunc(templateData);
        const destFile = `${publicFolderPath}/${htmlFile}`;
        generatedGitIgnoreFiles.push(htmlFile);
        fs.writeFile(destFile, compiledTemplate, ['utf8'], () => { });
    }

    for (const redirect of projectData.redirects) {
        if (!fs.existsSync(`${publicFolderPath}/${redirect.pattern}`)) {
            fs.mkdirSync(`${publicFolderPath}/${redirect.pattern}`);
        }

        const template = await readFile('./template/handlebar/redirect.hbs', 'utf8');
        const templateFunc = Handlebars.compile(template);
        const templateData = {
            title: redirect.pattern,
            url: redirect.url
        };

        const compiledTemplate = templateFunc(templateData);
        const destFile = `${publicFolderPath}/${redirect.pattern}/index.html`;
        generatedGitIgnoreFiles.push(`${redirect.pattern}/index.html`);
        fs.writeFile(destFile, compiledTemplate, ['utf8'], () => { });
    }

    fs.writeFile(`${publicFolderPath}/.gitignore`, generatedGitIgnoreFiles.join('\n'), ['utf8'], () => { });
}

require('dotenv').config();

buildTemplates()