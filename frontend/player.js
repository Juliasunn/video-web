

// variables
const content = document.getElementById("content");

var video;
var contentArr = [];

const fetchVideo = async () => {
    console.log("Fetch player media content called");
    
    // Adapted from examples on the Querystring homepage.
    console.log("This page url is: ", document.location.href);
    var parsedUrl = queryString(document.location.href);
    var resourceUuid =  parsedUrl["v"];
    console.log("value of param v: ", resourceUuid); 	
   // const resourceUuid = localStorage.getItem('resourceUuid');
    if (!resourceUuid) {
    	    return;
    }	
    const response = await fetch("http://127.0.0.1:8082/api/video/watch?v=" + resourceUuid); // "http://127.0.0.1:8081/video/b5cc0596-a647-4db2-b635-3a5aae5ac655"

    if (response.status >=200 && response.status < 300) {
        video = await response.json();
    } else {
        console.log(response.status, response.statusText);
        return;
    }
    fetchChannel(video.channelUuid);
    loadPageElements();
}
      
function loadPageElements() {
    console.log("Load elements called");
    const headerElement = document.getElementById("header");
    const descriptionElement = document.getElementById("description");
        
    const playerElement = document.getElementById("videoPlayer");
    const playerSrc = document.getElementById("videoSrc");
          
      
    headerElement.innerHTML = video.header;
    descriptionElement.innerHTML = video.description;
        
    playerElement.pause();
    playerSrc.setAttribute("src", "http://127.0.0.1:8081/api/media" + video.videoUrl);
    playerElement.load();
    playerElement.play();  
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
    fetchVideo();
});
