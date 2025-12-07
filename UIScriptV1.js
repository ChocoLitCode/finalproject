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

// GRAPH PANEL TOGGLE
const graphPanel = document.getElementById("graphPanel");

document.getElementById("graphToggle").addEventListener("click", () => {
    graphPanel.classList.add("open");
});

document.getElementById("graphClose")?.addEventListener("click", () => {
    graphPanel.classList.remove("open");
});

function toggleInfoPanel() {
    document.getElementById("infoPanel").classList.toggle("open");
}

let websocket;
const temperatureElement = document.getElementById("temperature");
const humidityElement   = document.getElementById("humidity");
const wsStatusElement   = document.getElementById("wsStatus");
const refreshBtn        = document.getElementById("refreshBtn");
const lastUpdateElement = document.getElementById("lastUpdateTime");
const tempTimeElement   = document.getElementById("tempTime");
const humidTimeElement  = document.getElementById("humidTime");

// Room 1 elements
const room1OnBtn    = document.getElementById("room1On");
const room1OffBtn   = document.getElementById("room1Off");
const room1AutoBtn  = document.getElementById("room1Auto");
const room1StateTxt = document.getElementById("room1State");

// Room 2 elements
const room2OnBtn    = document.getElementById("room2On");
const room2OffBtn   = document.getElementById("room2Off");
const room2AutoBtn  = document.getElementById("room2Auto");
const room2StateTxt = document.getElementById("room2State");

function getCurrentTime() {
    return new Date().toLocaleTimeString();
}

function updateTimeDisplay(target) {
    if (target) target.textContent = getCurrentTime();
}

function initWebSocket() {
    const gateway = `ws://${window.location.hostname}/ws`;
    websocket = new WebSocket(gateway);

    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen() {
    wsStatusElement.textContent = "Connected";
    wsStatusElement.className = "status-connected";
}

function onClose() {
    wsStatusElement.textContent = "Disconnected";
    wsStatusElement.className = "status-disconnected";

    setTimeout(initWebSocket, 2000); // Reconnect
}

function onMessage(event) {
    try {
        const data = JSON.parse(event.data);

        if (data.temperature !== undefined) {
            temperatureElement.textContent = data.temperature.toFixed(1);
            updateTimeDisplay(tempTimeElement);
        }

        if (data.humidity !== undefined) {
            humidityElement.textContent = data.humidity.toFixed(1);
            updateTimeDisplay(humidTimeElement);
        }

        if (data.room1 !== undefined) {
            room1StateTxt.textContent = data.room1;
        }

        // ★ ROOM 2 LIVE UPDATE (Added)
        if (data.room2 !== undefined) {
            room2StateTxt.textContent = data.room2;
        }

        lastUpdateElement.textContent = getCurrentTime();

    } catch (err) {
        console.log("WS Parse Error:", err);
    }
}

// Manual refresh
if (refreshBtn) {
    refreshBtn.addEventListener("click", () => {
        if (websocket && websocket.readyState === WebSocket.OPEN) {
            websocket.send("getReadings");
        }
    });
}

// ROOM 1 COMMANDS
function sendRoom1Command(cmd) {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
        websocket.send("room1:" + cmd);
        room1StateTxt.textContent = cmd;
    }
}

room1OnBtn?.addEventListener("click", () => sendRoom1Command("ON"));
room1OffBtn?.addEventListener("click", () => sendRoom1Command("OFF"));
room1AutoBtn?.addEventListener("click", () => sendRoom1Command("AUTO"));

// ROOM 2 COMMANDS
function sendRoom2Command(cmd) {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
        websocket.send("room2:" + cmd);
        room2StateTxt.textContent = cmd;
    }
}

room2OnBtn?.addEventListener("click", () => sendRoom2Command("ON"));
room2OffBtn?.addEventListener("click", () => sendRoom2Command("OFF"));
room2AutoBtn?.addEventListener("click", () => sendRoom2Command("AUTO"));


window.addEventListener("load", () => {
    initWebSocket();

    updateTimeDisplay(tempTimeElement);
    updateTimeDisplay(humidTimeElement);
    lastUpdateElement.textContent = getCurrentTime();

    // live ticking last update clock
    setInterval(() => {
        lastUpdateElement.textContent = getCurrentTime();
    }, 1000);
});
