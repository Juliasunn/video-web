//import {createWatches, createVideoImg} from "elements.js"

// variables
const userContent = document.getElementById("userContent");

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
    	const response = await fetch(ALL_URL);
    	contentArr = [];
    	if(response.status >=200 && response.status < 300) {
            const myJson = await response.json();
            contentArr = myJson;
            displayContent();
            return;
        }
    } catch (e) { }
    userContent.innerHTML = "<h5>No data found.</h5>"
}



function displayContent() {


    userContent.innerHTML = "";

    // if(newsDataArr.length == 0) {
    //     newsdetails.innerHTML = "<h5>No data found.</h5>"
    //     return;
    // }

    contentArr.forEach(news => {   
        var cardObj = new UsersVideoCard(news, "15%");
        console.log("-");
        var col = cardObj.getCard();
        console.log("--");    

        userContent.appendChild(col);
    });

}
