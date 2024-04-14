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
