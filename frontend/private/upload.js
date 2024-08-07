$("#myForm").submit(function(e) {
    e.preventDefault(); // avoid to execute the actual submit of the form.
    var form = $(this);

    // Create an FormData object 
    var sendData = new FormData( $('#myForm')[0]);
   
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
           fetchAll();
        }
    });
    
});



window.onload = async function() {
    const tokenCookie = getCookie("token");
    if (tokenCookie) {
        console.log("Token = ", tokenCookie);
        const claims = parseJwt(tokenCookie);
        const channelUuid = claims.sub;
        //fetchVideoFeed(document.getElementById("userContent"), "20%", "?ch="+channelUuid);
        const channelVideo = await fetchVideoFeed("?ch="+channelUuid);
        console.log("Channel video = ", channelVideo)
        displayUsersContent(document.getElementById("userContent"), "20%", channelVideo)
    }
    var uploadBox = document.getElementById("uploadBox");
    
    var videoUploadForm = document.createElement('video-upload-form-component');
    videoUploadForm.setAttribute('id', "videoUploadForm");
    uploadBox.appendChild(videoUploadForm);
    
   // var streamUploadForm = document.createElement('stream-upload-form-component');
   // streamUploadForm.setAttribute('id', "streamUploadForm");
   // uploadBox.appendChild(streamUploadForm);
    
};
