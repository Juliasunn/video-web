$("#myForm").submit(function(e) {
    console.log("[Attach ajax handler to form]")

    e.preventDefault(); // avoid to execute the actual submit of the form.

    var form = $(this);

    // Create an FormData object 
    var sendData = new FormData( $('#myForm')[0]);
   
    $.ajax({
        type: "POST",
        url: "http://127.0.0.1:8082/api/profile",
        enctype: 'multipart/form-data',
        processData: false,  // Important!
        contentType: false,
        cache: false,
        data: sendData,
        success: function(data)
        {
           console.log('FORM SUBMITTED! ', data);
           fetchProfile();
          // window.open(window.location.href,'_self');
        }
    });
    
}); 


window.onload = function() {
    console.log("onload");
    fetchProfile();
};

const fetchProfile = async () => {
    console.log("[fetchProfile]");
    try {
    	const response = await fetch("http://127.0.0.1:8082/api/profile");
    	//contentArr = [];
    	if(response.status >=200 && response.status < 300) {
            const profileJson = await response.json();
            console.log("Profile: ", profileJson);
           // contentArr = myJson;
            fillForm(profileJson);
            return;
        }
    } catch (e) {  console.log("Failed to fetch profile."); }
   
    //userContent.innerHTML = "<h5>No data found.</h5>"
}

function fillForm(profileJson) {
    	const inputName = document.getElementById('inputName');
	const inputLogin = document.getElementById('inputLogin');
    	const inputMail = document.getElementById('inputMail');
	const inputPhone = document.getElementById('inputPhone');
    	const inputAbout = document.getElementById('inputAbout');
    	
    	const labelName = document.getElementById('labelName');
	const labelLogin = document.getElementById('labelLogin');
	const avatarImg = document.getElementById('avatarImg');    	
    	const userJson = profileJson.user;
    	const subjectJson = profileJson.subject;
    	
    	inputName.value = userJson.name;
    	inputLogin.value = subjectJson.login;
    	inputMail.value = subjectJson.mail;
    	inputPhone.value = subjectJson.phone;    	
    	inputAbout.value = userJson.about;
    	
    	labelName.innerHTML = userJson.name;
    	labelLogin.innerHTML = subjectJson.login;
    	avatarImg.src = userJson.avatarImgUrl;
}
