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

window.onload = function() {
    fetchVideoFeed(document.getElementById("userContent"), "20%");
};
