

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
    const response = await fetch("http://127.0.0.1:8081" +"/video/watch" + "?v=" + resourceUuid); // "http://127.0.0.1:8081/video/b5cc0596-a647-4db2-b635-3a5aae5ac655"

    if (response.status >=200 && response.status < 300) {
        video = await response.json();
    } else {
        console.log(response.status, response.statusText);
        return;
    }
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
    playerSrc.setAttribute("src", "http://127.0.0.1:8081" + video.videoUrl);
    playerElement.load();
    playerElement.play();  
}

function loadChannelInfo() {
    const mediaColumn = document.getElementById("mediaColumn");
    
    const ChannelInfo = new DescrptionChannel({});
    var channelInfoHtml = ChannelInfo.getHtml();
    mediaColumn.appendChild(channelInfoHtml);  
}

const fetchVideos = async () => {
    console.log("fetchAll called");
    try {
    	const response = await fetch("http://127.0.0.1:8081/video/feed");
    	contentArr = [];
    	if(response.status >=200 && response.status < 300) {
            const myJson = await response.json();
            contentArr = myJson;
            displayContent();
            return;
        }
    } catch (e) { }
    content.innerHTML = "<h5>No data found.</h5>"
}


function displayContent() {
    content.innerHTML = "";

     if(contentArr.length == 0) {
         content.innerHTML = "<h5>No data found.</h5>"
         return;
     }
    contentArr.forEach(videoJson => {
        console.log("-Append card-");
        var cardObj = new PublicVideoCard(videoJson, "100%");
        console.log("-");
        var col = cardObj.getCard();
        console.log("--");
        content.appendChild(col);
    });
}
      
window.addEventListener('load', () => {
    fetchVideo();
    loadChannelInfo();
    fetchVideos();
});
