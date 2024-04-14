const watchesIcon = `<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-eye-fill" viewBox="0 0 16 16">
  <path d="M10.5 8a2.5 2.5 0 1 1-5 0 2.5 2.5 0 0 1 5 0"/>
  <path d="M0 8s3-5.5 8-5.5S16 8 16 8s-3 5.5-8 5.5S0 8 0 8m8 3.5a3.5 3.5 0 1 0 0-7 3.5 3.5 0 0 0 0 7"/>
</svg>`

const editIcon = `
<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="currentColor" class="bi bi-pencil-square" viewBox="0 0 20 20">
  <path d="M15.502 1.94a.5.5 0 0 1 0 .706L14.459 3.69l-2-2L13.502.646a.5.5 0 0 1 .707 0l1.293 1.293zm-1.75 2.456-2-2L4.939 9.21a.5.5 0 0 0-.121.196l-.805 2.414a.25.25 0 0 0 .316.316l2.414-.805a.5.5 0 0 0 .196-.12l6.813-6.814z"/>
  <path fill-rule="evenodd" d="M1 13.5A1.5 1.5 0 0 0 2.5 15h11a1.5 1.5 0 0 0 1.5-1.5v-6a.5.5 0 0 0-1 0v6a.5.5 0 0 1-.5.5h-11a.5.5 0 0 1-.5-.5v-11a.5.5 0 0 1 .5-.5H9a.5.5 0 0 0 0-1H2.5A1.5 1.5 0 0 0 1 2.5z"/>
</svg>`

function createWatches(watchesCount) {
    var iconShow = document.createRange().createContextualFragment(watchesIcon)
    var watches = document.createElement('p');
    watches.className="text-truncate text-muted m-0 text-right";
    watches.appendChild(iconShow);
    watches.innerHTML += watchesCount;
    return watches;    
}

function createVideoImg(imgSrc) {
    var image = document.createElement('img');
    image.setAttribute("height","matchparent");
    image.setAttribute("width","100%");
    image.src = imgSrc;
    console.log("image src = ",imgSrc);
    return image;   
}


class BasicVideoCard {
  #col;
  #card;
  #cardBody;
  
  constructor( videoJson, width ) {
        this.#col = document.createElement('div');
        this.#col.className="p-2 card";
        this.#col.style="width: "+width;
        
        this.#card = document.createElement('div');
        this.#card.className = "p-2";
        
        this.#cardBody = document.createElement('div');
        
        var preview = createVideoImg("http://127.0.0.1:8082" + videoJson.previewImgUrl);
        
        var videoHeader = document.createElement('p');
        videoHeader.className = "card-title";
        videoHeader.innerHTML = videoJson.header;
        
        this.#cardBody.appendChild(videoHeader);
        this.#card.appendChild(preview);
        this.#card.appendChild(this.#cardBody);

        this.#col.appendChild(this.#card);                
  }
  
  appendToCard( el ) {
      this.#card.appendChild(el);
  }
  
  appendToCardBody( el ) {
      this.#cardBody.appendChild(el);
  } 
  
  getCard() {
      return this.#col;
  } 
}

class PublicVideoCard extends BasicVideoCard {
  #infoRow;
  #watchBtn;
  
  constructor(videoJson, width) {
        super(videoJson, width)
        console.log("finished parent");
        
        this.#infoRow = document.createElement('div');
        this.#infoRow.className = "d-flex justify-content-around";
        
        var channelObj = new BaseChannel({}, "30px", "30px");
        var channel = channelObj.getHtml();
        
      //  var avatar = document.createElement('img');
      //  avatar.setAttribute("height","30px");
      //  avatar.setAttribute("width","30px");
      //  avatar.src = "http://127.0.0.1:8082" + "/avatar/basic.jpg";
        
        var watches = createWatches("15 тыс.");
        
      //  var channel = document.createElement('p');
      //  channel.className="text-muted";
      //  channel.innerHTML = "Vicky Potts";
        
        //this.#infoRow.appendChild(avatar);
        this.#infoRow.appendChild(channel);
        this.#infoRow.appendChild(watches);
        
        this.#watchBtn = document.createElement('a');
        this.#watchBtn.href = "http://127.0.0.1:8082" + "/player"+"?v="+videoJson.uuid;
        console.log("href = ", this.#watchBtn.href);
        this.#watchBtn.target = "_self";
        this.#watchBtn.className="btn btn-dark text-light";
        this.#watchBtn.innerHTML="Watch";
        
        var resourceUuid = videoJson.uuid;
        console.log("resource uuid = ",resourceUuid);
        
     //   this.#watchBtn.addEventListener("click",function(){
//
 //       	localStorage.setItem('resourceUuid', videoJson.uuid);
 //       	console.log("Set resourceUuid variable to: ", localStorage.getItem('resourceUuid'));
 //       	window.open("http://127.0.0.1:8082" + "/player" );
        	//console.log("Open page uri: ", STATIC_URL + PLAYER_PATH);
     	
//	});
	
	super.appendToCardBody( this.#infoRow );
	super.appendToCardBody( this.#watchBtn );            
  }
  
  getCard() {
      return super.getCard();
  }   
}

class UsersVideoCard extends BasicVideoCard  {
  #infoRow;
  #controlRow;
  constructor(videoJson, width) {
    super(videoJson, width)
    this.#infoRow = document.createElement('div');
    this.#infoRow.className="d-flex justify-content-around";
        
    var date = document.createElement('p');
    date.className="text-truncate text-muted m-0";
    date.innerHTML = "28.12.2019";
             
    var watches = createWatches("1235");

       
    this.#infoRow.appendChild(date);
    this.#infoRow.appendChild(watches);
        
    var iconEdit = document.createRange().createContextualFragment(editIcon);


    this.#controlRow = document.createElement('div');
    this.#controlRow.className="d-flex justify-content-around";
    this.#controlRow.appendChild(iconEdit);
    
    var link = document.createElement('a');
    link.className="stretched-link"
    link.href="#"
    link.appendChild(iconEdit);
    super.appendToCard( link )
    
    super.appendToCardBody( this.#infoRow );
    super.appendToCardBody( this.#controlRow ); 
 }
    
  getCard() {
      return super.getCard();
  } 
  
}


