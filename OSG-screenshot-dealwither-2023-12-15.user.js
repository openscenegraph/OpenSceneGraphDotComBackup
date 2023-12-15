// ==UserScript==
// @name         OSG screenshot dealwither
// @namespace    http://tampermonkey.net/
// @version      2023-12-15
// @description  try to take over the world!
// @author       You
// @match        http*://www.openscenegraph.com/index.php*
// @icon         https://www.google.com/s2/favicons?sz=64&domain=openscenegraph.com
// @grant        none
// ==/UserScript==

(function() {
    'use strict';

    localStorage.setItem('trying', window.location.href);

    let knownUrls = new Set(JSON.parse(localStorage.getItem('osgScreenshotsKnown')) ?? []);

    window.open = (url, target, windowFeatures) => {
        if (target && target != "_self" ) {
            console.log(`target ${target}`);
        }
        if (windowFeatures) {
            console.log(`windowFeatures ${windowFeatures}`);
        }
        console.log(new URL(url, window.location));
        knownUrls.add(new URL(url, window.location).href);
    };
    window.print = console.log;

    var ayyys = document.getElementsByTagName("a");

    for (let ayy of ayyys) {
        if (ayy.onclick) {
            localStorage.setItem('aboutToClick', JSON.stringify(ayy));
            console.log(ayy);
            ayy.onclick();
        }
    };

    localStorage.setItem('finished', window.location.href);
    localStorage.setItem('trying', undefined);

    localStorage.setItem('osgScreenshotsKnown', JSON.stringify([...knownUrls]));

    let triedUrls = new Set(JSON.parse(localStorage.getItem('osgScreenshotsTried')) ?? []);
    triedUrls.add(window.location.href);
    localStorage.setItem('osgScreenshotsTried', JSON.stringify([...triedUrls]));

    let nextUrl = null;
    for (let next of knownUrls) {
        if (!triedUrls.has(next)) {
            nextUrl = next;
            break;
        }
    }
    if (nextUrl == null) {
        window.alert("Done!");
    } else {
        console.log(`leaving ${window.location.href} and going to ${nextUrl}`);
        window.location = nextUrl;
    }
})();
