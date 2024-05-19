class FileUploadInput extends HTMLElement {
  constructor() {
    super();
  }
    
  connectedCallback() {
    this.innerHTML = ` 
<style>    
    input[type="file"] 
    {
        display: none;
    }
    .custom-upload 
    {
        border: none;
        border-radius: 10px;
        color: white;
        background-color: orange;

        padding: 10px;
        cursor: pointer;          
    }
    
    .custom-upload:hover 
    {
        background-color: orange;         
    }            
</style>            
    
  </head>
  <body>

    <div id="uploadGroup">
      <label id="uploadInputText" class="custom-upload">
        <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-arrow-bar-up" viewBox="0 0 16 16"> <path fill-rule="evenodd" d="M8 10a.5.5 0 0 0 .5-.5V3.707l2.146 2.147a.5.5 0 0 0 .708-.708l-3-3a.5.5 0 0 0-.708 0l-3 3a.5.5 0 1 0 .708.708L7.5 3.707V9.5a.5.5 0 0 0 .5.5m-7 2.5a.5.5 0 0 1 .5-.5h13a.5.5 0 0 1 0 1h-13a.5.5 0 0 1-.5-.5"/>
        </svg>
        Select image
        <input class="form-control" type="file" id="inputFile" name="inputFile"> </input>
      </label>
      <span id="fileChosenText">No file chosen</span> 
    </div>


  </body>
    `;
     console.log("SCRIPT LOADED!")
     const actualBtn = document.getElementById('inputFile');
     const fileChosen = document.getElementById('fileChosenText');
	
     actualBtn.addEventListener('change', function(){
	    console.log("loaded file");
  	    fileChosen.textContent = this.files[0].name
    })
    }
}
    
customElements.define('file-upload-component', FileUploadInput);

