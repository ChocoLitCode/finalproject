document.addEventListener("DOMContentLoaded", () => {
    const darkToggle = document.getElementById("darkToggle");
    darkToggle.addEventListener("click", () => {
        document.body.classList.toggle("dark-mode");
    });
});
