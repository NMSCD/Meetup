const fs = require('fs');

async function downloadScssFiles() {
    const files = [
        { dest: './template/scss/third-party/_pico.scss', url: 'https://cdn.jsdelivr.net/npm/@picocss/pico@1/css/pico.min.css' },
        { dest: './template/scss/third-party/_normalize.scss', url: 'https://necolas.github.io/normalize.css/8.0.1/normalize.css' },
        // { dest: './template/scss/third-party/_simple-grid.scss', url: 'https://simplegrid.io/grid/simple-grid.css' }, // Editted to remove conflicting styles for font and body
    ];

    for (const fileObj of files) {
        const cssResponse = await fetch(fileObj.url, { method: 'GET' });
        const cssBody = await cssResponse.text();
        fs.writeFile(fileObj.dest, cssBody, ['utf8'], () => { });
    }
}

downloadScssFiles();