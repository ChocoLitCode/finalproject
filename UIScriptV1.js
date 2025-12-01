// Dark mode toggle
document.getElementById("darkToggle").addEventListener("click", () => {
    document.body.classList.toggle("dark-mode");
});

// Notification panel toggle
const notifPanel = document.getElementById("notifPanel");
document.getElementById("notifToggle").addEventListener("click", () => {
    notifPanel.classList.add("open");
});
document.getElementById("notifClose").addEventListener("click", () => {
    notifPanel.classList.remove("open");
});