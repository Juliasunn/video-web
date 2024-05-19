

const fetchVideoFeed = async (containerHtml, widthHtml) => {
    console.log("fetch video feed");
    var videoJsonArray = [];
    try {
    	const fetchVideos = await fetch("http://127.0.0.1:8082/api/video/feed");
    	if(fetchVideos.status >=200 && fetchVideos.status < 300) {
            videoJsonArray = await fetchVideos.json();
        }
    } catch (e) {
        console.log("Error: ", e); 
    }
    displayContent(containerHtml, widthHtml, videoJsonArray);
}

function displayContent(containerHtml, widthHtml,  videoJsonArray) {

    containerHtml.innerHTML = "";

     if(videoJsonArray.length == 0) {
         containerHtml.innerHTML = "<h5>No data found.</h5>"
         return;
     }

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
