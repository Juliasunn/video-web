// variables
const generalBtn = document.getElementById("genral");
//const accountBtn = document.getElementById("user");

const uploadBtn = document.getElementById("upload");
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
const ALL_URL = "http://127.0.0.1:8081/video/feed";

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
    pageFrame.src = "http://127.0.0.1:8082/upload";
});

accountBtn.addEventListener("click",function(){
 	console.log("displayRegister called");
 	pageFrame.src = "http://127.0.0.1:8082/registration";
});

const fetchAll = async () => {
    console.log("fetchAll called");
    const response = await fetch(ALL_URL);
    newsDataArr = [];
    if(response.status >=200 && response.status < 300) {
        const myJson = await response.json();
        newsDataArr = myJson;
    } else {
        // handle errors
        console.log(response.status, response.statusText);
        newsdetails.innerHTML = "<h5>No data found.</h5>"
        return;
    }
    displayNews();
}

function displayNews() {

    newsdetails.innerHTML = "";

    // if(newsDataArr.length == 0) {
    //     newsdetails.innerHTML = "<h5>No data found.</h5>"
    //     return;
    // }

    newsDataArr.forEach(news => {
        
        var col = document.createElement('div');
        col.className="col-sm-12 col-md-4 col-lg-3 p-2 card";

        var card = document.createElement('div');
        card.className = "p-2";

        var image = document.createElement('img');
        image.setAttribute("height","matchparent");
        image.setAttribute("width","100%");
        image.src = STATIC_URL + news.previewImgUrl;
        console.log("image src = ", image.src);

        var cardBody = document.createElement('div');
        
        var newsHeading = document.createElement('h5');
        newsHeading.className = "card-title";
        newsHeading.innerHTML = news.header;

        var discription = document.createElement('p');
        discription.className="text-muted";
        discription.innerHTML = news.description;

        var link = document.createElement('a');
        link.className="btn btn-dark";
        //В какое окно загружается документ (по умолчанию в текущее)
        //link.setAttribute("target", "_blank");
        //link.href = SERVER_URL + "/video/player";
        link.innerHTML="Watch";
        var resourceUuid = news.uuid;
        console.log("resource uuid = ", news);
        
        link.addEventListener("click",function(){

        	localStorage.setItem('resourceUuid', resourceUuid);
        	console.log("Set resourceUuid variable to: ", localStorage.getItem('resourceUuid'));
        	window.open(STATIC_URL + PLAYER_PATH);
        	console.log("Open page uri: ", STATIC_URL + PLAYER_PATH);
	});

        cardBody.appendChild(newsHeading);
        cardBody.appendChild(discription);
        cardBody.appendChild(link);

        card.appendChild(image);
        card.appendChild(cardBody);

        col.appendChild(card);

        newsdetails.appendChild(col);
    });

}

