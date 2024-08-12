

// variables
const content = document.getElementById("content");

//var video;
var contentArr = [];


const parseResource = async (response) => {
    if (response.status >=200 && response.status < 300) {
        var parsed = await response.json();
        return parsed;
    } else {
        console.log(response.status, response.statusText);
        return {};
    }
}

/* Video or stream */
const fetchResource = async () => {
    const universalPlayerElement = document.getElementById("universalPlayer");
    console.log("Fetch player media content called");
    
    // Adapted from examples on the Querystring homepage.
    console.log("This page url is: ", document.location.href);
    var parsedUrl = queryString(document.location.href);
    
    var channelJson = {};
    
    if (parsedUrl["v"]) {
        const response = await fetch("http://127.0.0.1:8082/api/video/watch?v=" + parsedUrl["v"]);
        const video = await parseResource( response );
        if (!video) {
            return;
        }
        channelJson = await fetchChannel(video.channelUuid);
        loadPageElements(video.header, video.description, video.videoUrl, channelJson);
        universalPlayerElement.currentVideo = video;             

    } else if (parsedUrl["s"]) {
        const response = await fetch("http://127.0.0.1:8082/api/stream/watch?s=" + parsedUrl["s"]);
        const stream = await parseResource( response );
        if (!stream) {
            return;
        }
        channelJson = await fetchChannel(stream.channelUuid);
        loadPageElements(stream.name, stream.description, stream.uuid, channelJson);
        universalPlayerElement.currentStream = stream;  
    } else {
        console.log("Error: Path dont containt stream('s') and video('v') params.")
    }
    
}
      
function loadPageElements(header, description, source, channel) {
    console.log("Load elements called");
    const headerElement = document.getElementById("header");
    const descriptionElement = document.getElementById("description");
    headerElement.innerHTML = header;
    descriptionElement.innerHTML = description;
    
    const ChannelInfo = new DescrptionChannel(channel);
    var channelInfoHtml = ChannelInfo.getHtml();
    mediaColumn.appendChild(channelInfoHtml); 
 
}
      
window.addEventListener('load', async () => {
    fetchVideoFeed().then((allVideo) => {
        displayContent(document.getElementById("content"), "100%", allVideo)
    });
    fetchResource();
});
