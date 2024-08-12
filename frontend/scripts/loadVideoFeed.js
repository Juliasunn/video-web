

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

const fetchStreamFeed = async (channelUuid) => {
    console.log("[loadStreams]");
    var streamJsonArray = [];
    try {
    	const fetchStreams = await fetch("http://127.0.0.1:8082/api/stream/feed?ch="+channelUuid);
    	if (fetchStreams.status >=200 && fetchStreams.status < 300) {
            const streamJson = await fetchStreams.json();
            
            if (Object.keys(streamJson).length) {
                 console.log("Stream: ", streamJson);
                 streamJsonArray.push(streamJson);
            }
        }
    } catch (e) { 
        console.log("Failed to fetch stream ", e);
    }
    return streamJsonArray;
}

const fetchChannel = async (channelUuid) => {
        var channelJson = {};
        try {
            const fetchChannel = await fetch("http://127.0.0.1:8082/api/channel/" + channelUuid);
        
            if (fetchChannel.status >=200 && fetchChannel.status < 300) {
               channelJson = await fetchChannel.json();
            } else {
               console.log(fetchChannel.status, fetchChannel.statusText);
           }
        } catch (e) {
         console.log("Error: ", e); 
        }
        return channelJson;
}

function displayContent(containerHtml, widthHtml,  videoJsonArray, streamJsonArray = []) {

    containerHtml.innerHTML = "";
     
    streamJsonArray.forEach(stream = async (stream) => {
        const channelJson = await fetchChannel(stream.channelUuid)
    
        console.log("-Append card-");
        var cardObj = new PublicStreamCard(stream, channelJson, widthHtml);
        var col = cardObj.getCard();
        containerHtml.appendChild(col);
    });

    videoJsonArray.forEach(video = async (video) => {
       const channelJson = await fetchChannel(video.channelUuid)
    
        console.log("-Append card-");
        var cardObj = new PublicVideoCard(video, channelJson, widthHtml);
        var col = cardObj.getCard();
        containerHtml.appendChild(col);
    });

}

/* Show content on other channel's content page */
function displayContentOnChannel(containerHtml, widthHtml,  channelUuid, videoJsonArray, streamJsonArray = []) {

    containerHtml.innerHTML = "";
    fetchChannel(channelUuid).then((channelJson) => {
    	streamJsonArray.forEach(stream = (stream) => {
        	var cardObj = new PublicStreamCard(stream, channelJson, widthHtml);
        	var col = cardObj.getCard();
        	containerHtml.appendChild(col);
    	});

    	videoJsonArray.forEach(video = (video) => {
        	var cardObj = new PublicVideoCard(video, channelJson, widthHtml);
        	var col = cardObj.getCard();
        	containerHtml.appendChild(col);
    	});
    
    });
}

/* Show content on user's own content page */
function displayUsersContent(containerHtml, widthHtml,  videoJsonArray) {

    containerHtml.innerHTML = "";

     if(videoJsonArray.length == 0) {
         containerHtml.innerHTML = "<h5>No content found.</h5>"
         return;
     }

    videoJsonArray.forEach(video = (video) => {

        console.log("-Append card-");
        var cardObj = new UsersVideoCard(video, widthHtml);
        var col = cardObj.getCard();
        containerHtml.appendChild(col);
    });

}
