document.addEventListener("DOMContentLoaded", () => {
    const obj = document.getElementById("darkToggle");

    obj.addEventListener("load", () => {
        obj.contentDocument.documentElement.addEventListener("click", () => {
            document.body.classList.toggle("dark-mode");
        });
    });
});