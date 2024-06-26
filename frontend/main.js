
window.onload = async function() {
    const allVideo = await fetchVideoFeed();
    displayContent(document.getElementById("content"), "20%", allVideo)
};

