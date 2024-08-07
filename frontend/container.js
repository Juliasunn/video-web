// variables
const generalBtn = document.getElementById("genral");
//const accountBtn = document.getElementById("user");

const uploadBtn = document.getElementById("upload");
const streamBtn = document.getElementById("stream");
const accountBtn = document.getElementById("account");
const searchBtn = document.getElementById("searchBtn");

const newsQuery = document.getElementById("newsQuery");
const newsType = document.getElementById("newsType");
const newsdetails = document.getElementById("pageContent");


pageFrame = document.getElementById("iframe");

// Array
var newsDataArr = [];

// apis 
const STATIC_URL = "http://127.0.0.1:8082";
const SERVER_URL = "http://127.0.0.1:8081";
const ALL_URL = "http://127.0.0.1:8082/api/video/feed";

const PLAYER_PATH = "/player";
const PREVIEW_PATH = "/preview";

function resizeIframe(obj){
     obj.setAttribute("width","100%");
     obj.setAttribute("height","100%");
}

window.onload = function() {
    console.log("onload");
};

generalBtn.addEventListener("click",function(){
    console.log("display content page.");
    pageFrame.src = "http://127.0.0.1:8082/main";
});

uploadBtn.addEventListener("click",function(){
    console.log("display upload page.");
    pageFrame.src = "http://127.0.0.1:8082/private/upload";
});

streamBtn.addEventListener("click",function(){
    console.log("display stream managment page.");
    pageFrame.src = "http://127.0.0.1:8082/private/stream";
});

accountBtn.addEventListener("click",function(){
 	console.log("displayRegister called");
 	pageFrame.src = "http://127.0.0.1:8082/private/editprofile";
});


