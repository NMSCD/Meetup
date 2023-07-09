<div align="center">

# 🏠 Home page

The home page of the No Man's Sky Community Developers & Designers.

</div>

<br>

## Building the project locally

Many of the files are generated using Handlebar templates. To get the solution running, use the following commands:

- `npm run setup`
  - This will install all npm packages and then create the **templates/data/live.json** file with all the members of the NMSCD
- `npm run dev`

The dev script runs 3 processes concurrently. This shouldn't be a problem, but if you want to run them individually, they are:

- `dev:watch-template`
  - This builds all of the handlebar files using the JSON files in the `data` folder
- `dev:watch-sass`
  - This compiles all the SCSS files into `public/assets/css/main.css`
- `dev:live-server`
  - A small HTTP server. Once running you should see the site on [127.0.0.1:8080](https://127.0.0.1:8080)

<br>

## Deployment

This site makes use of Github Actions to build the project and Github Pages to host it 💪


<!-- Links used in the page -->

[nmscd]: https://github.com/NMSCD?ref=nmscd
