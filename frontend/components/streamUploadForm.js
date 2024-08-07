class StreamUploadForm extends HTMLElement {


  #streamToShow

  constructor() {
    super();
  }
    
  connectedCallback() {
    this.innerHTML = ` 
           
  <body >
    <div class="mt-0 row p-2 d-flex justify-content-center align-items-center h-100 w-100">
         <form id="streamForm">

            <div class="form-group mb-3">
               <label for="name">Stream name</label>
               <input type="text" class="form-control" id="name" name="name" placeholder="Name to publish">
               <small id="emailHelp" class="form-text text-muted">Stream name should catch the audience's eye.</small>
            </div>
            
            <div class="form-group mb-3">
               <label for="description">Stream description</label>
               <textarea id="description" class="form-control " name="description" placeholder="Enter your stream description..." style="height:200px"></textarea>
            </div>
            
            <div class="form-group mb-3">
               <label for="streamTime">Stream duration</label>
               <input type="text" class="form-control" id="streamTime" name="streamTime" placeholder=" -/- " readonly>
               <small id="emailHelp" class="form-text text-muted">This stream will be available during this period.</small>
            </div>
            
            <div class="form-group mb-3">
               <label for="streamUrl">Stream URL</label>
               <input type="text" class="form-control" id="streamUrl" name="firstname" placeholder=" -/- " readonly>
               <small id="emailHelp" class="form-text text-muted">This stream will be available via (for 3h since beginning) this link.</small>
            </div>
            
            
            <div class="form-group mb-3">
               <label for="streamKey">Stream key</label>
               <input type="text" class="form-control" id="streamKey" name="streamKey" placeholder=" -/- " readonly>
               <small id="emailHelp" class="form-text text-muted">Use this key in your OBS settings.</small>
            </div>         
            
            <br>
            <input id="submitBtn" type="submit" class="btn btn-warning ms-2 float-right" value="Create">
         </form>
         
         <div class="d-flex w-25 m-5">
            <!-- <div class="col-md-10 col-lg-6 col-xl-7 d-flex align-items-center order-1 order-lg-2"> -->
            <stream-player-component id="streamPlayer"></stream-player-component>          
         </div>
    </div>
         
  </body>
    `;
     console.log("Connected callback");
    this.#streamToShow = {};
    var submitBtn = document.getElementById("submitBtn");
    fetch("http://127.0.0.1:8082/api/stream/my").then((streamResponse) => {
     	if(streamResponse.status >=200 && streamResponse.status < 300) {      
            streamResponse.json().then( (stream) => {
                console.log("Stream: ", stream);
                if (stream) {
                	this.#streamToShow = stream;
                       this.showCurrentStream();                
                 } else {
                       this.#streamToShow = {};
                       submitBtn.value="Start";
                 }
            });
         }
        var form = document.getElementById("streamForm");
        async function handleSubmit(event, streamToShow) {
             console.log("PREVENT DEFAULT")
             event.preventDefault();
            
             var sendData = new FormData(event.target);
            console.log("Process submit action with ajax jquery library ", sendData);
    
            var actionType = "POST";
            if ( streamToShow ) {
                actionType = "PUT";
                sendData.set("streamKey", streamToShow.uuid)
            }
            $.ajax({
                type: actionType,
                url: "http://127.0.0.1:8082/api/stream/my",
                enctype: 'multipart/form-data',
                processData: false,  // Important!
                contentType: false,
                cache: false,
                data: sendData,
                success: function(data)
                {
                   console.log('FORM SUBMITTED! ', data);
                  // window.open(window.location.href,'_self');
                }
            });
    
        }
        handleSubmit.bind(null, this.#streamToShow);
        form.addEventListener("submit", handleSubmit)
         
         
    });
    
  } //Connected callback
    
  
   showCurrentStream() {
       var streamUrlHtml = document.getElementById("streamUrl");
       var streamKeyHtml = document.getElementById("streamKey");
       var streamNameHtml = document.getElementById("name");
       var streamDescriptionHtml = document.getElementById("description");
       //var streamPreviewHtml = document.getElementById("previewUploader");
       var streamPlayer = document.getElementById("streamPlayer");
       var streamTime = document.getElementById("streamTime");
       var submitBtn = document.getElementById("submitBtn");
                         
       streamNameHtml.value = this.#streamToShow.name;
       streamNameHtml.setAttribute("readonly", "readonly");
       streamDescriptionHtml.value = this.#streamToShow.description;
       streamDescriptionHtml.setAttribute("readonly", "readonly");
           
       streamUrlHtml.value = "http://127.0.0.1:8082/streams/" + this.#streamToShow.uuid;
       streamKeyHtml.value = this.#streamToShow.uuid + "?key=" + this.#streamToShow.publishKey;
                                  

       streamTime.value = (this.#streamToShow.start + " - " + this.#streamToShow.expire);
       //streamPreviewHtml.style.display = 'none'
       streamPlayer.currentStream = this.#streamToShow;
       submitBtn.value="Finish"              
   }
    
}
    
customElements.define('stream-upload-form-component', StreamUploadForm);
