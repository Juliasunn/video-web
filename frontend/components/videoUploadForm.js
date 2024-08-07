class VideoUploadForm extends HTMLElement {

  constructor() {
    super();
  }
    
  connectedCallback() {
    this.innerHTML = ` 
           
  <body>
  <div class="mt-0 row p-2 d-flex justify-content-center align-items-center h-100 w-100">
        <form id="videoForm">
            <div class="form-group mb-3">
               <label>Select videofile</label>
               <file-upload-component></file-upload-component>
            </div>
            <div class="form-group mb-3">
               <label for="fname">Video name *</label>
               <input type="text" class="form-control" id="fname" name="firstname" placeholder="Name to publish">
               <small id="emailHelp" class="form-text text-muted">Video with no name provided will be published with the same name as uploaded file.</small>
            </div>
            <div class="form-group mb-3">
               <label for="description">Enter description</label>
               <textarea id="description" class="form-control " name="description" placeholder="Enter your video description..." style="height:200px"></textarea>
            </div>
            <br>
            <input type="submit" class="btn btn-warning ms-2 float-right" value="Submit">
         </form>
         <div class="d-flex w-25 m-5">
            <!-- <div class="col-md-10 col-lg-6 col-xl-7 d-flex align-items-center order-1 order-lg-2"> -->
            <img src="http://127.0.0.1:8082/img/camera_pic.png"
               class="img-fluid" alt="Sample image">
         </div>
  </div>
  </body>
    `;
     console.log("Connected callback");
     
     var form = document.getElementById("videoForm");
     async function handleSubmit(event, streamToShow) {
         console.log("PREVENT DEFAULT")
         event.preventDefault();
            
         var sendData = new FormData(event.target);
         console.log("Process submit action with ajax jquery library ", sendData);
    
         $.ajax({
             type: "POST",
             url: "http://127.0.0.1:8082/api/upload",
             enctype: 'multipart/form-data',
             processData: false,  // Important!
             contentType: false,
             cache: false,
             data: sendData,
             success: function(data)
             {
                console.log('FORM SUBMITTED! ');
                //fetchAll();
             }
         });
     }
     form.addEventListener("submit", handleSubmit)    
    
    } //connectedCallback()
    
}
    
customElements.define('video-upload-form-component', VideoUploadForm);
