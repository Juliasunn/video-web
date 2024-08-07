

const fetchAndDisplayVideoFeed = async (containerHtml, widthHtml, queryParamsString = "") => {
    console.log("fetch video feed");
    var videoJsonArray = await fetchVideoFeed(queryParamsString)
    displayContent(containerHtml, widthHtml, videoJsonArray);
}


const fetchVideoFeed = async (queryParamsString = "") => {
    console.log("fetch video feed");
    var videoJsonArray = [];
    try {
    	const fetchVideos = await fetch("http://127.0.0.1:8082/api/video/feed"+ queryParamsString);
    	console.log("uri: ", "http://127.0.0.1:8082/api/video/feed"+ queryParamsString);
    	if(fetchVideos.status >=200 && fetchVideos.status < 300) {
            videoJsonArray = await fetchVideos.json();
        }
    } catch (e) {
        console.log("Error: ", e); 
    }
    return videoJsonArray;
}

function displayContent(containerHtml, widthHtml,  videoJsonArray, streamJsonArray = []) {

    containerHtml.innerHTML = "";

     if(videoJsonArray.length == 0) {
         containerHtml.innerHTML = "<h5>No data found.</h5>"
         return;
     }
     
    streamJsonArray.forEach(stream = async (stream) => {
        var channelJson = {};
        try {
            const fetchChannel = await fetch("http://127.0.0.1:8082/api/channel/" + stream.channelUuid);
        
            if (fetchChannel.status >=200 && fetchChannel.status < 300) {
               channelJson = await fetchChannel.json();
            } else {
               console.log(fetchChannel.status, fetchChannel.statusText);
           }
        } catch (e) {
         console.log("Error: ", e); 
        }
    
        console.log("-Append card-");
        var cardObj = new PublicStreamCard(stream, channelJson, widthHtml);
        var col = cardObj.getCard();
        containerHtml.appendChild(col);
    });

    videoJsonArray.forEach(video = async (video) => {
        var channelJson = {};
        try {
            const fetchChannel = await fetch("http://127.0.0.1:8082/api/channel/" + video.channelUuid);
        
            if (fetchChannel.status >=200 && fetchChannel.status < 300) {
               channelJson = await fetchChannel.json();
            } else {
               console.log(fetchChannel.status, fetchChannel.statusText);
           }
        } catch (e) {
         console.log("Error: ", e); 
        }
    
        console.log("-Append card-");
        var cardObj = new PublicVideoCard(video, channelJson, widthHtml);
        var col = cardObj.getCard();
        containerHtml.appendChild(col);
    });

}

function displayUsersContent(containerHtml, widthHtml,  videoJsonArray) {

    containerHtml.innerHTML = "";

     if(videoJsonArray.length == 0) {
         containerHtml.innerHTML = "<h5>No data found.</h5>"
         return;
     }

    videoJsonArray.forEach(video = async (video) => {
        var channelJson = {};

    
        console.log("-Append card-");
        var cardObj = new UsersVideoCard(video, widthHtml);
        var col = cardObj.getCard();
        containerHtml.appendChild(col);
    });

}
