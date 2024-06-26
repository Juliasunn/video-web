function queryString(url) { 
    const str1 = url.split('?')[1]; 
    const params = {}; 
  
    if (str1) { 
        const pairs = str1.split('&'); 
        for (const pair of pairs) { 
            const [key, value] = pair.split('='); 
            params[key] = decodeURIComponent(value 
                          .replace(/\+/g, ' ')); 
        } 
    } 
  
    return params; 
}

function getCookie(name) {
  const value = `; ${document.cookie}`;
  const parts = value.split(`; ${name}=`);
  if (parts.length === 2) return parts.pop().split(';').shift();
}

function parseJwt (token) {
    var base64Url = token.split('.')[1];
    var base64 = base64Url.replace(/-/g, '+').replace(/_/g, '/');
    var jsonPayload = decodeURIComponent(window.atob(base64).split('').map(function(c) {
        return '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2);
    }).join(''));

    return JSON.parse(jsonPayload);
}
