

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
    
    if (parsedUrl["v"]) {
        const response = await fetch("http://127.0.0.1:8082/api/video/watch?v=" + parsedUrl["v"]);
        const video = await parseResource( response );
        if (!video) {
            return;
        }
        fetchChannel(video.channelUuid);
        loadPageElements(video.header, video.description, video.videoUrl);
        universalPlayerElement.currentVideo = video;             

    } else if (parsedUrl["s"]) {
        const response = await fetch("http://127.0.0.1:8082/api/stream/watch?s=" + parsedUrl["s"]);
        const stream = await parseResource( response );
        if (!stream) {
            return;
        }
        fetchChannel(stream.channelUuid);
        loadPageElements(stream.name, stream.description, stream.uuid);
        universalPlayerElement.currentStream = stream;  
    } else {
        console.log("Error: Path dont containt stream('s') and video('v') params.")
    }
}
      
function loadPageElements(header, description, source) {
    console.log("Load elements called");
    const headerElement = document.getElementById("header");
    const descriptionElement = document.getElementById("description");
        
  //  const playerElement = document.getElementById("videoPlayer");
   // const playerSrc = document.getElementById("videoSrc");
          
      
    headerElement.innerHTML = header;
    descriptionElement.innerHTML = description;
        
  //  playerElement.pause();
   // playerSrc.setAttribute("src", "http://127.0.0.1:8081/api/media" + source);
   // playerElement.load();
   // playerElement.play();  
}

const fetchChannel = async (channelUuid) => {
    console.log("fetchChannel called ", channelUuid);
    var channel = {};
    try {	
       const response = await fetch("http://127.0.0.1:8082/api/channel/" + channelUuid); // "http://127.0.0.1:8081/video/b5cc0596-a647-4db2-b635-3a5aae5ac655"

       if (response.status >=200 && response.status < 300) {
           channel = await response.json();
       } else {
           console.log(response.status, response.statusText);
           return;
       }
    } catch (e) { console.log("Error: ", e) }
    console.log("FETCHED CHANNEL:", channel)
    loadChannelInfo(channel)
}

function loadChannelInfo(channel) {
    const mediaColumn = document.getElementById("mediaColumn");
    
    const ChannelInfo = new DescrptionChannel(channel);
    var channelInfoHtml = ChannelInfo.getHtml();
    mediaColumn.appendChild(channelInfoHtml);  
}
      
window.addEventListener('load', async () => {
    const allVideo = await fetchVideoFeed();
    displayContent(document.getElementById("content"), "100%", allVideo)
   // fetchVideoFeed(document.getElementById("content"), "100%");
    fetchResource();
});
