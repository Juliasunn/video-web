class UniversalPlayer extends HTMLElement {

  #currentVideo;
  #currentStream;
  #width = 0;
  #height = 0;
  #poster = 0;
   
  constructor() {
    super();
  }
    
  connectedCallback() {
       console.log("width:"+this.#width+" height: "+ this.#height)

     console.log("[UniversalPlayer] connected callback");
     
    this.innerHTML = ` 
   <script src="https://cdn.jsdelivr.net/npm/hls.js@latest"></script>   
  <body>
    <video id="videoPlayer" width="75%" controls preload="none" muted="muted">
     
    </video>
  </body>
  <script src="https://cdn.jsdelivr.net/npm/hls.js@latest"></script>
    `;
    if (this.#width) {
        document.getElementById('videoPlayer').setAttribute('width', this.#width);
    }
    if (this.#height) {
        document.getElementById('videoPlayer').setAttribute('height', this.#height);
    }
    if (this.#poster) {
        document.getElementById('videoPlayer').setAttribute('poster', this.#poster);
    }
    }
    
  attributeChangedCallback(name, oldValue, newValue) {
    console.log(`Attribute ${name} has changed: `+oldValue+"->"+newValue+" "+this.#width + " " +this.#height);
    if (name == 'width') {
        this.#width = newValue;
     }
    if (name == 'height') {
         this.#height = newValue;
    }
    if (name == 'poster') {
        this.#poster = newValue;
    }
    var playerElement = document.getElementById('videoPlayer');
    if (playerElement) {
         playerElement.setAttribute(name, newValue);
    }
        
 
  }
  
  static get observedAttributes() { // (3)
    return ['width', 'height', 'poster'];
  }
  
  //It seems like 'get' method is required to change attribute's value (is called before).
  get width() {
     return this.#width
  }
  get height() {
       return this.#height
  }
  
  get poster() {
       return this.#poster
  }
  
  //It seems like 'set' methods never been called, attributeChangedCallback is called instead.
  set width( value ) {
     console.log(`[set width]`);
     this.#width = value
  }
 
  set height( value ) {
     console.log(`[set height]`);
     this.#height = value
  }
  
  set poster( value ) {
     console.log(`[set poster]`);
     this.#poster = value
  }
  
  set currentVideo( video ) {
       console.log(`[UniversalPlayer] set currentVideo.`);
      this.#currentVideo = video;
      this.loadVideo();
  }
  
  set currentStream( stream ) {
       console.log(`[UniversalPlayer] set currentStream.`);
      this.#currentStream = stream;
      this.loadStream();
  }
  
  
  loadVideo() {
    console.log("[UniversalPlayer] load video");
    const playerElement = document.getElementById("videoPlayer");
    playerElement.pause();
    
    var playerSrc = document.getElementById("videoSrc");
    if (!playerSrc) {
        playerSrc = document.createElement('source');
        playerSrc.setAttribute('id', "videoSrc");
        playerSrc.setAttribute('type', "video/mp4");
        playerElement.appendChild(playerSrc);
    }
    playerSrc.setAttribute("src", "http://127.0.0.1:8081/api/media" + this.#currentVideo.videoUrl);
    playerElement.load();
    playerElement.play(); 
  }
 
  loadStream() {
      console.log("[UniversalPlayer] load stream");
      var video = document.getElementById("videoPlayer");
      video.setAttribute('autoplay', true)
      //video.setAttribute('poster', "http://127.0.0.1:8082/img/stream_default2.jpg")
      this.setAttribute('poster', "http://127.0.0.1:8082/img/stream_default2.jpg")
      
      if (!this.#currentStream) {
          return;
      }
      var serverUrl = "http://127.0.0.1:8082/streams"
      var hlsVideoSrc = "/hls/" + this.#currentStream.uuid + ".m3u8";
      var mpdVideoSrc = "/dash/"+ this.#currentStream.uuid + ".mpd";    
      
      if (Hls.isSupported()) {
         console.log("[Media type]: is supported 'hls'");
        
         var hls = new Hls();
         
         var sourceUrl = serverUrl+hlsVideoSrc;
         var sourceReloadRetry = (function (url, event, data) {
            console.log("Error occured during source loading: ", event, data, url);
            setTimeout( function retry() { 
                console.log("Invoke reload source.")
                hls.loadSource(url);
         }, 5000);
        }).bind(null, sourceUrl);
        
        
         hls.loadSource(sourceUrl);
	 hls.on(Hls.Events.ERROR, sourceReloadRetry);
         hls.attachMedia(video);
      }
      // hls.js is not supported on platforms that do not have Media Source
      // Extensions (MSE) enabled.
      //
      // When the browser has built-in HLS support (check using `canPlayType`),
      // we can provide an HLS manifest (i.e. .m3u8 URL) directly to the video
      // element through the `src` property. This is using the built-in support
      // of the plain video element, without using hls.js.
      //
      // Note: it would be more normal to wait on the 'canplay' event below however
      // on Safari (where you are most likely to find built-in HLS support) the
      // video.src URL must be on the user-driven white-list before a 'canplay'
      // event will be emitted; the last video event that can be reliably
      // listened-for when the URL is not on the white-list is 'loadedmetadata'.
      else if (video.canPlayType("application/vnd.apple.mpegurl")) {
        console.log("[Media type]: can play 'application/vnd.apple.mpegurl'");
        video.src = serverUrl+hlsVideoSrc;
      }
      
      else {
        console.log("[Media type]: dash");
        video.src = serverUrl+mpdVideoSrc;
      }
  } 
  
}
    
customElements.define('universal-player-component', UniversalPlayer);
