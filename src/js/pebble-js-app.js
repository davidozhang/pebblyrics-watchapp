function sendMessage(response) {
    console.log(response);
}

function httpGetLyricsAsync(artist, track) {
    var url = 'http://restling-pebblyrics.rhcloud.com/lyrics?artist=' + artist + '&track=' + track;
    console.log(url);
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
            sendMessage(xmlHttp.responseText);
    }
    xmlHttp.open("GET", url, true);
    xmlHttp.send(null);
}

Pebble.addEventListener('ready', function(e) {
    console.log('App Loaded');
});
												
Pebble.addEventListener('appmessage', function(e) {
    var artist = e.payload.artist;
    var track = e.payload.track;
    var artist_query = artist.replace(/\s+/g, '_');
    var track_query = track.replace(/\s+/g, '_');
    httpGetLyricsAsync(artist_query, track_query);
});