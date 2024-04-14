// variables
const content = document.getElementById("content");

// Array
var contentArr = [];

// apis 
const STATIC_URL = "http://127.0.0.1:8082";
const SERVER_URL = "http://127.0.0.1:8081";
const ALL_URL = "http://127.0.0.1:8081/video/feed";

const PLAYER_PATH = "/player";
const PREVIEW_PATH = "/preview";

window.onload = function() {
    console.log("onload");
    fetchAll();
};

const fetchAll = async () => {
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

    contentArr.forEach(news => {
        console.log("-Append card-");
        var cardObj = new PublicVideoCard(news, "15%");
        console.log("-");
        var col = cardObj.getCard();
        console.log("--");
        content.appendChild(col);
    });

}
