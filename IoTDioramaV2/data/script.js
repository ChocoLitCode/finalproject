/* ------------------------------
   script.js - ESP32 DHT22 Monitor
   ------------------------------ */

let websocket = null;
let reconnectDelay = 2000;
const maxReconnectDelay = 5000;

// DOM Elements
const dom = {
    temperature: document.getElementById('temperature'),
    humidity: document.getElementById('humidity'),
    wsStatus: document.getElementById('wsStatus'),
    refreshBtn: document.getElementById('refreshBtn'),
    lastUpdateTime: document.getElementById('lastUpdateTime'),
    tempTime: document.getElementById('tempTime'),
    humidTime: document.getElementById('humidTime'),
    room1Status: document.getElementById('room1Status'),
    room1State: document.getElementById('room1State'),
    room2Status: document.getElementById('room2Status'),
    room2State: document.getElementById('room2State'),
    room1On: document.getElementById('room1On'),
    room1Off: document.getElementById('room1Off'),
    room1Auto: document.getElementById('room1Auto'),
    room2On: document.getElementById('room2On'),
    room2Off: document.getElementById('room2Off'),
    room2Auto: document.getElementById('room2Auto')
};

// Alert panel UI
const alertPanel = document.getElementById('alertPanel');
const alertMessage = document.getElementById('alertMessage');
const dismissAlert = document.getElementById('dismissAlert');

function showAlert(message, autoHideMs = 0) {
    alertMessage.textContent = message;
    alertPanel.classList.remove('hidden');
    alertPanel.classList.add('visible');
    if (autoHideMs > 0) {
        setTimeout(hideAlert, autoHideMs);
    }
}

function hideAlert() {
    alertPanel.classList.remove('visible');
    alertPanel.classList.add('hidden');
}

dismissAlert.addEventListener('click', hideAlert);

// ------------------------------
// Chart Setup
// ------------------------------
const MAX_POINTS = 30;
let chartData = { temp: [], humid: [], labels: [] };

function createGradient(ctx, color) {
    const gradient = ctx.createLinearGradient(0, 0, 0, 200);
    gradient.addColorStop(0, color);
    gradient.addColorStop(1, 'rgba(255,255,255,0)');
    return gradient;
}

const ctx = document.getElementById('combinedChart').getContext('2d');
const combinedChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: chartData.labels,
        datasets: [
            {
                label: 'Temperature (°C)',
                data: chartData.temp,
                borderColor: 'rgba(231,76,60,1)',
                backgroundColor: createGradient(ctx, 'rgba(231,76,60,0.4)'),
                tension: 0.4,
                fill: true,
                pointRadius: 2
            },
            {
                label: 'Humidity (%)',
                data: chartData.humid,
                borderColor: 'rgba(52,152,219,1)',
                backgroundColor: createGradient(ctx, 'rgba(52,152,219,0.4)'),
                tension: 0.4,
                fill: true,
                pointRadius: 2
            }
        ]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: { duration: 500, easing: 'easeOutQuart' },
        plugins: { legend: { position: 'top' } },
        scales: { x: { title: { display: true, text: 'Time' } }, y: { beginAtZero: true } }
    }
});

function updateChart(temp, humid) {
    const time = new Date().toLocaleTimeString();
    chartData.labels.push(time);
    chartData.temp.push(temp);
    chartData.humid.push(humid);

    if (chartData.labels.length > MAX_POINTS) {
        chartData.labels.shift();
        chartData.temp.shift();
        chartData.humid.shift();
    }

    combinedChart.update();
}

// ------------------------------
// Time helpers
// ------------------------------
function getCurrentTime() { return new Date().toLocaleTimeString(); }
function updateTimeStamp(element) { element.textContent = getCurrentTime(); }

// ------------------------------
// UI helpers
// ------------------------------
function updateRoomUI(room, state) {
    if (room === 'room1') {
        dom.room1Status.textContent = state;
        dom.room1State.textContent = state;
    } else if (room === 'room2') {
        dom.room2Status.textContent = state;
        dom.room2State.textContent = state;
    }
}

function setControlsEnabled(enabled) {
    [
        dom.room1On, dom.room1Off, dom.room1Auto,
        dom.room2On, dom.room2Off, dom.room2Auto,
        dom.refreshBtn
    ].forEach(btn => btn.disabled = !enabled);
}

// ------------------------------
// WebSocket
// ------------------------------
function initWebSocket() {
    const gateway = `ws://${window.location.hostname}/ws`;
    try { websocket = new WebSocket(gateway); }
    catch(e) { console.error('WebSocket creation failed', e); scheduleReconnect(); return; }

    websocket.onopen = () => {
        console.log('WebSocket connected');
        dom.wsStatus.textContent = 'Connected';
        dom.wsStatus.className = 'status-connected';
        setControlsEnabled(true);
        websocket.send('getReadings');
        reconnectDelay = 2000;
    };

    websocket.onclose = () => {
        console.log('WebSocket disconnected');
        dom.wsStatus.textContent = 'Disconnected';
        dom.wsStatus.className = 'status-disconnected';
        setControlsEnabled(false);
        scheduleReconnect();
    };

    websocket.onerror = (err) => { console.warn('WebSocket error', err); };

    websocket.onmessage = onMessage;
}

function scheduleReconnect() {
    if (websocket) { try { websocket.close(); } catch(e){} websocket=null; }
    const delay = Math.min(reconnectDelay, maxReconnectDelay);
    console.log(`Reconnecting in ${delay} ms...`);
    setTimeout(() => {
        initWebSocket();
        reconnectDelay = Math.min(maxReconnectDelay, reconnectDelay * 2.5);
    }, delay);
}

// ------------------------------
// Message handling
// ------------------------------
// ------------------------------
// Message handling (revised)
// ------------------------------
function onMessage(event) {
    try {
        const data = JSON.parse(event.data);

        // --- Temperature & Humidity ---
        const temp = parseFloat(data.temperature ?? data.temp);
        const humid = parseFloat(data.humidity ?? data.h);

        // Only update if valid numbers
        if (!isNaN(temp)) {
            dom.temperature.textContent = temp.toFixed(1);
            updateTimeStamp(dom.tempTime);
        }
        if (!isNaN(humid)) {
            dom.humidity.textContent = humid.toFixed(1);
            updateTimeStamp(dom.humidTime);
        }

        // Update chart only if we have valid readings
        if (!isNaN(temp) && !isNaN(humid)) {
            updateChart(temp, humid);
        }

        // --- Room States ---
        if (data.room1 !== undefined) updateRoomUIIfChanged('room1', String(data.room1));
        else if (data.room1_state !== undefined) updateRoomUIIfChanged('room1', String(data.room1_state));

        if (data.room2 !== undefined) updateRoomUIIfChanged('room2', String(data.room2));
        else if (data.room2_state !== undefined) updateRoomUIIfChanged('room2', String(data.room2_state));

        // --- Alert Handling ---
        if (data.alert) {
            if (data.alert === 'Access Granted') showAlert(data.alert, 4000);
            else showAlert(data.alert, 0); // Access Denied stays until dismiss
        }

        dom.lastUpdateTime.textContent = getCurrentTime();
    } catch(e) {
        console.error('Error parsing WebSocket message', e, event.data);
    }
}

// ------------------------------
// Room UI helpers (prevent unnecessary updates)
// ------------------------------
function updateRoomUIIfChanged(room, newState) {
    if (room === 'room1') {
        if (dom.room1Status.textContent !== newState) {
            dom.room1Status.textContent = newState;
            dom.room1State.textContent = newState;
        }
    } else if (room === 'room2') {
        if (dom.room2Status.textContent !== newState) {
            dom.room2Status.textContent = newState;
            dom.room2State.textContent = newState;
        }
    }
}


// ------------------------------
// Manual refresh
// ------------------------------
function refreshReadings() {
    if (websocket?.readyState === WebSocket.OPEN) websocket.send('getReadings');
    else console.warn('WebSocket not open - cannot refresh');
}

// ------------------------------
// Room Control
// ------------------------------
function sendRoomCommand(room, command) {
    if (websocket?.readyState !== WebSocket.OPEN) {
        console.warn('WebSocket not ready - ignoring command', room, command);
        return;
    }
    websocket.send(`${room}:${command}`);
    // DO NOT call updateRoomUI here — wait for ESP32 feedback
}



// ------------------------------
// UI Event Binding
// ------------------------------
window.addEventListener('load', () => {
    setControlsEnabled(false);
    initWebSocket();

    dom.refreshBtn.addEventListener('click', refreshReadings);

    updateTimeStamp(dom.tempTime);
    updateTimeStamp(dom.humidTime);
    dom.lastUpdateTime.textContent = getCurrentTime();

    dom.room1On.addEventListener('click', () => sendRoomCommand('room1', 'ON'));
    dom.room1Off.addEventListener('click', () => sendRoomCommand('room1', 'OFF'));
    dom.room1Auto.addEventListener('click', () => sendRoomCommand('room1', 'AUTO'));

    dom.room2On.addEventListener('click', () => sendRoomCommand('room2', 'ON'));
    dom.room2Off.addEventListener('click', () => sendRoomCommand('room2', 'OFF'));
    dom.room2Auto.addEventListener('click', () => sendRoomCommand('room2', 'AUTO'));
});
