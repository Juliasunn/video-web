const watchesIcon = `<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-eye-fill" viewBox="0 0 16 16">
  <path d="M10.5 8a2.5 2.5 0 1 1-5 0 2.5 2.5 0 0 1 5 0"/>
  <path d="M0 8s3-5.5 8-5.5S16 8 16 8s-3 5.5-8 5.5S0 8 0 8m8 3.5a3.5 3.5 0 1 0 0-7 3.5 3.5 0 0 0 0 7"/>
</svg>`

const editIcon = `
<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="currentColor" class="bi bi-pencil-square" viewBox="0 0 20 20">
  <path d="M15.502 1.94a.5.5 0 0 1 0 .706L14.459 3.69l-2-2L13.502.646a.5.5 0 0 1 .707 0l1.293 1.293zm-1.75 2.456-2-2L4.939 9.21a.5.5 0 0 0-.121.196l-.805 2.414a.25.25 0 0 0 .316.316l2.414-.805a.5.5 0 0 0 .196-.12l6.813-6.814z"/>
  <path fill-rule="evenodd" d="M1 13.5A1.5 1.5 0 0 0 2.5 15h11a1.5 1.5 0 0 0 1.5-1.5v-6a.5.5 0 0 0-1 0v6a.5.5 0 0 1-.5.5h-11a.5.5 0 0 1-.5-.5v-11a.5.5 0 0 1 .5-.5H9a.5.5 0 0 0 0-1H2.5A1.5 1.5 0 0 0 1 2.5z"/>
</svg>`


const trashIcon =`<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-trash" viewBox="0 0 16 16">
  <path d="M5.5 5.5A.5.5 0 0 1 6 6v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5m2.5 0a.5.5 0 0 1 .5.5v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5m3 .5a.5.5 0 0 0-1 0v6a.5.5 0 0 0 1 0z"/>
  <path d="M14.5 3a1 1 0 0 1-1 1H13v9a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V4h-.5a1 1 0 0 1-1-1V2a1 1 0 0 1 1-1H6a1 1 0 0 1 1-1h2a1 1 0 0 1 1 1h3.5a1 1 0 0 1 1 1zM4.118 4 4 4.059V13a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1V4.059L11.882 4zM2.5 3h11V2h-11z"/>
</svg>`

const broadcastIcon = `<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="currentColor" class="bi bi-broadcast-pin" viewBox="0 0 20 20">
  <path d="M3.05 3.05a7 7 0 0 0 0 9.9.5.5 0 0 1-.707.707 8 8 0 0 1 0-11.314.5.5 0 0 1 .707.707m2.122 2.122a4 4 0 0 0 0 5.656.5.5 0 1 1-.708.708 5 5 0 0 1 0-7.072.5.5 0 0 1 .708.708m5.656-.708a.5.5 0 0 1 .708 0 5 5 0 0 1 0 7.072.5.5 0 1 1-.708-.708 4 4 0 0 0 0-5.656.5.5 0 0 1 0-.708m2.122-2.12a.5.5 0 0 1 .707 0 8 8 0 0 1 0 11.313.5.5 0 0 1-.707-.707 7 7 0 0 0 0-9.9.5.5 0 0 1 0-.707zM6 8a2 2 0 1 1 2.5 1.937V15.5a.5.5 0 0 1-1 0V9.937A2 2 0 0 1 6 8"/>
</svg>`

function createWatches(watchesCount) {
    var iconShow = document.createRange().createContextualFragment(watchesIcon)
    var watches = document.createElement('p');
    watches.className="text-truncate text-muted m-0 text-right";
    watches.appendChild(iconShow);
    watches.innerHTML += watchesCount;
    return watches;    
}

function createBroadcast(text) {
    var icon = document.createRange().createContextualFragment(broadcastIcon)
    var iconWithText = document.createElement('p');
    iconWithText.className="text-truncate text-muted m-0 text-right";
    iconWithText.appendChild(icon);
    iconWithText.innerHTML += text;
    return iconWithText;    
}

function createVideoImg(imgSrc) {
    var image = document.createElement('img');
    image.setAttribute("width","100%");
    image.setAttribute("height", "matchparent");
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
        this.#card.className = "p-2 card justify-content-between";
        this.#card.style = "height: 100%";
        
        this.#cardBody = document.createElement('div');
        
        var preview = createVideoImg("http://127.0.0.1:8082" + videoJson.previewImgUrl);
        

        var videoHeader = document.createElement('a');
        videoHeader.className="fw-bolder text-dark";
        videoHeader.style = "font-weight:bold"
        videoHeader.rel="stylesheet"
        videoHeader.href = "http://127.0.0.1:8082" + "/player"+"?v="+videoJson.uuid
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

  constructor(videoJson, channelJson, width) {
        super(videoJson, width)
        console.log("finished parent");
        
        this.#infoRow = document.createElement('div');
        this.#infoRow.className = "d-flex justify-content-around";
        
        var channelObj = new BaseChannel(channelJson, "30px", "30px");
        var channel = channelObj.getHtml();
        
        var watches = createWatches("15 тыс.");
       
        this.#infoRow.appendChild(channel);
        this.#infoRow.appendChild(watches);
        
      //  this.#watchBtn = document.createElement('a');
     //   this.#watchBtn.href = "http://127.0.0.1:8082" + "/player"+"?v="+videoJson.uuid;
       // console.log("href = ", this.#watchBtn.href);
      //  this.#watchBtn.target = "_self";
      //  this.#watchBtn.className="btn btn-dark text-light";
       // this.#watchBtn.innerHTML="Watch";
        
        var resourceUuid = videoJson.uuid;
        console.log("resource uuid = ",resourceUuid);
	
	super.appendToCardBody( this.#infoRow );         
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
    
    var deleteBtn = document.createElement('button');
    deleteBtn.addEventListener('click', async _ => {
    	try {     
        	const response = await fetch("http://127.0.0.1:8082/api/video/"+ videoJson.uuid, {
        	method: 'delete'
        	});
        	console.log('Video deleted!', response);
    	} catch(err) {
        	console.error(`Error while delete video: ${err}`);
    	}
   });
   deleteBtn.className="btn btn-danger text-light m-1";
   deleteBtn.innerHTML=trashIcon;


    this.#controlRow = document.createElement('div');
    this.#controlRow.className="d-flex justify-content-between";
    this.#controlRow.appendChild(deleteBtn);
    
   // var link = document.createElement('a');
   // link.className="stretched-link"
   // link.href="#"
   // link.appendChild(iconEdit);
   // super.appendToCard( link )
    
    super.appendToCard( this.#infoRow );
    super.appendToCard( this.#controlRow ); 
 }
    
  getCard() {
      return super.getCard();
  } 
  
}

class BasicStreamCard {
  #col;
  #card;
  #cardBody;
  
  constructor( streamJson, width ) {
        this.#col = document.createElement('div');
        this.#col.className="p-2 card";
        this.#col.style="width: "+width;
        
        this.#card = document.createElement('div');
        this.#card.className = "p-2 card justify-content-between";
        this.#card.style = "height: 100%";
        
        this.#cardBody = document.createElement('div');
        
        var preview = createVideoImg("http://127.0.0.1:8082/img/stream_default2.jpg");
        

        var streamHeader = document.createElement('a');
        streamHeader.className="fw-bolder text-dark";
        streamHeader.style = "font-weight:bold"
        streamHeader.rel="stylesheet"
        streamHeader.href = "http://127.0.0.1:8082" + "/player"+"?s="+streamJson.uuid
        streamHeader.innerHTML = streamJson.name;
        
        this.#cardBody.appendChild(streamHeader);
        this.#card.appendChild(preview);
        this.#card.appendChild(this.#cardBody);

        this.#col.appendChild(this.#card);                
  }
  
  appendToCardBody( el ) {
      this.#cardBody.appendChild(el);
  } 
  
  getCard() {
      return this.#col;
  } 
}

class PublicStreamCard extends BasicStreamCard {
  #infoRow;

  constructor(streamJson, channelJson, width) {
        super(streamJson, width)
        console.log("finished parent");
        
        this.#infoRow = document.createElement('div');
        this.#infoRow.className = "d-flex justify-content-around";
        
        var channelObj = new BaseChannel(channelJson, "30px", "30px");
        var channel = channelObj.getHtml();
        
        var aboutStream = createBroadcast("Online");
  
        this.#infoRow.appendChild(channel);
        this.#infoRow.appendChild(aboutStream);
	super.appendToCardBody( this.#infoRow );         
  }
  
  getCard() {
      return super.getCard();
  }   
}


