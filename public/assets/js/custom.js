const milliInSecond = 1000;
const milliInMinute = 60000;
const milliInHour = 3600000;
const milliInDay = 86400000;
const countdownTimeoutDur = 250;

let countdownTextState = 'Loading...';

const registerServiceWorker = async () => {
    if ('serviceWorker' in navigator) {
        try {
            const registration = await navigator.serviceWorker.register('/serviceWorker.js');
            if (registration.installing) {
                console.log('Service worker installing');
            } else if (registration.waiting) {
                console.log('Service worker installed');
            } else if (registration.active) {
                console.log('Service worker active');
            }
        } catch (error) {
            console.error(`Registration failed with ${error}`);
        }
    }
};

const highlightHtml = (char) => `<span class="secondary opacity-80">${char}</span>`;

const calcDiffInMilli = (startDate) => {
    let diffInMilli = startDate - Date.now();
    if (diffInMilli < 0) {

        if (Math.abs(diffInMilli) < (milliInHour * 4)) {
            return { status: 'live', milli: 0 };
        }

        return { status: 'over', milli: 0 };
    }

    return {
        status: 'countdown',
        milli: startDate - Date.now(),
    };
}

const calculateCountdown = (startDate) => {

    if (isNaN(startDate)) {
        updateCountdownText('');
        return;
    }

    const diffInMilliObj = calcDiffInMilli(startDate);
    if (diffInMilliObj.status != 'countdown') {

        if (diffInMilliObj.status == 'live') {
            updateCountdownText(highlightHtml('<span class="flip-x">🎉</span> It is happening right now! 🎉'));
        }

        if (diffInMilliObj.status == 'over') {
            updateCountdownText(highlightHtml('The event is over, join us next year!'));

            const planned = document.getElementById('event-planned');
            const unplanned = document.getElementById('event-unplanned');

            planned.classList.toggle('hidden');
            unplanned.classList.toggle('hidden');
        }

        return;
    }

    let diffInMilli = diffInMilliObj.milli;

    const contentArr = [];

    const days = Math.floor((diffInMilli / milliInDay));
    if (days > 0) {
        contentArr.push(`${days}${highlightHtml(' days')}`);
        diffInMilli = diffInMilli - (days * milliInDay);
    }

    const hours = Math.floor((diffInMilli / milliInHour));
    if (hours > 0) {
        contentArr.push(`${hours}${highlightHtml(' hours')}`);
        diffInMilli = diffInMilli - (hours * milliInHour);
    }

    const minutes = Math.floor((diffInMilli / milliInMinute));
    if (minutes > 0) {
        contentArr.push(`${minutes}${highlightHtml(' minutes')}`);
        diffInMilli = diffInMilli - (minutes * milliInMinute);
    }

    const seconds = Math.floor((diffInMilli / milliInSecond));
    contentArr.push(`${seconds}${highlightHtml(' seconds')}`);

    const innerHtml = contentArr.join('<span class="opacity-80"> - </span>');
    updateCountdownText(innerHtml);

    setTimeout(() => {
        calculateCountdown(startDate);
    }, countdownTimeoutDur);
}

const initVideo = () => {
    const bannerBgElem = document.getElementById('banner-bg');
    if (bannerBgElem == null) return;

    const video = document.createElement('video');
    video.src = '/assets/vid/banner.mp4';
    video.classList = 'opacity-transition-1s opacity-0';
    video.muted = true;
    video.autoplay = true;
    video.loop = true;
    video.disablePictureInPicture = true;
    video.controls = false;
    video.draggable = false;
    video.oncanplay = function () {
        video.classList.remove('opacity-0');
    };

    bannerBgElem.appendChild(video);

    // setTimeout(() => video.play(), 1000);
}

const updateCountdownText = (text) => {
    if (countdownTextState == text) return;

    const countdownHeader = document.getElementById('countdown');
    if (countdownHeader == null) return;

    countdownHeader.innerHTML = text;
}

const pageSetup = () => {
    const startDateStr = window.siteData.nextEventDate;
    const startDate = new Date(startDateStr);

    registerServiceWorker();
    calculateCountdown(startDate);
    // calculateCountdown(new Date('1 July 2023 12:01:00 GMT+0200'));
    setTimeout(() => initVideo(), 250);
}
pageSetup();
