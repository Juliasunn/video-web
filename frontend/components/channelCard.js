

class BaseChannel {
  #element;

  constructor( channelJson, w, h ) {
      console.log("Channel element constructor:", channelJson);
      this.#element = document.createElement('div');
      this.#element.className="d-flex mb-2"
  
      var avatar = document.createElement('img');
      avatar.setAttribute("height",w);
      avatar.setAttribute("width", h);
      avatar.src = "http://127.0.0.1:8082" + channelJson.avatarImgUrl;
      
      
      var channel = document.createElement('p');
      channel.className="text-muted  my-1";
      channel.innerHTML = channelJson.name
      
     this.#element.appendChild(avatar);
     this.#element.appendChild(channel); 
  }
  
  getHtml() {
      return this.#element;
  } 
}

class DescrptionChannel extends BaseChannel {
  #subscribes;
  #subscribeBtn;
  #element;
  
  constructor( channelJson ) {
      super(channelJson, "50px", "50px");
      this.#element = document.createElement('div');

  
      this.#subscribeBtn = document.createElement('a');
      this.#subscribeBtn.className="btn btn-dark text-light";
      this.#subscribeBtn.innerHTML="Subscribe";
           
      this.#subscribes = document.createElement('p');
      this.#subscribes.className="text-muted my-1";
      this.#subscribes.innerHTML = "13.1 тыс." + " подписчиков";
  }
   
  getHtml() {
      this.#element.appendChild(super.getHtml());
      this.#element.appendChild(this.#subscribes);
      this.#element.appendChild(this.#subscribeBtn);
      return  this.#element;
  } 

}
