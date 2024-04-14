

class BaseChannel {
  #element;

  
  constructor( channelJson, width ) {
      this.#element = document.createElement('div');
  
      var avatar = document.createElement('img');
      avatar.setAttribute("height","30px");
      avatar.setAttribute("width","30px");
      avatar.src = "http://127.0.0.1:8082" + "/avatar/basic.jpg";
      
      
      var channel = document.createElement('p');
      channel.className="text-muted";
      channel.innerHTML = "Vicky Potts";
      
     this.#element.appendChild(avatar);
     this.#element.appendChild(channel); 
  } 


}
