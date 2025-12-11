// Dark mode toggle
document.getElementById("darkToggle").addEventListener("click", () => {
    document.body.classList.toggle("dark-mode");
    updateChartTheme();
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
    // Initialize charts when panel opens
    if (!chartsInitialized) {
        initCharts();
        chartsInitialized = true;
    }
});
document.getElementById("graphClose")?.addEventListener("click", () => {
    graphPanel.classList.remove("open");
});


// ===== WebSocket Variables =====
let websocket = null;
let reconnectDelay = 2000;
const maxReconnectDelay = 5000;

// DOM Elements
const temperatureElement = document.getElementById("temperature");
const humidityElement = document.getElementById("humidity");
const tempTimeElement = document.getElementById("tempTime");
const humidTimeElement = document.getElementById("humidTime");
const soundBadgeElement = document.getElementById("soundBadge");
const soundTimeElement = document.getElementById("soundTime");

// Room 1 elements
const room1OnBtn = document.getElementById("room1On");
const room1OffBtn = document.getElementById("room1Off");
const room1AutoBtn = document.getElementById("room1Auto");
const room1StateTxt = document.getElementById("room1State");
const light1State = document.getElementById("light1State");

// Room 2 elements
const room2OnBtn = document.getElementById("room2On");
const room2OffBtn = document.getElementById("room2Off");
const room2AutoBtn = document.getElementById("room2Auto");
const room2StateTxt = document.getElementById("room2State");
const light2State = document.getElementById("light2State");

// ===== Notification System =====
let notifications = [];
const MAX_NOTIFICATIONS = 50;

function addNotification(type, message) {
    const timestamp = new Date().toLocaleString();
    const notification = {
        type: type,
        message: message,
        timestamp: timestamp,
        id: Date.now()
    };
    
    notifications.unshift(notification); // Add to beginning
    
    // Keep only last MAX_NOTIFICATIONS
    if (notifications.length > MAX_NOTIFICATIONS) {
        notifications = notifications.slice(0, MAX_NOTIFICATIONS);
    }
    
    displayNotifications();
    updateNotificationBadge();
}

function displayNotifications() {
    const notifContent = document.querySelector('.notif-content');
    
    if (notifications.length === 0) {
        notifContent.innerHTML = '<div class="notif-item">No new notifications.</div>';
        return;
    }
    
    let html = '';
    notifications.forEach(notif => {
        let icon = '';
        let className = 'notif-item';
        
        switch(notif.type) {
            case 'heat':
                icon = '🌡️';
                className += ' notif-warning';
                break;
            case 'intruder':
                icon = '🚨';
                className += ' notif-danger';
                break;
            case 'failed':
                icon = '⚠️';
                className += ' notif-warning';
                break;
            case 'access':
                icon = '✅';
                className += ' notif-success';
                break;
            default:
                icon = 'ℹ️';
        }
        
        html += `
            <div class="${className}">
                <div class="notif-icon">${icon}</div>
                <div class="notif-body">
                    <div class="notif-message">${notif.message}</div>
                    <div class="notif-time">${notif.timestamp}</div>
                </div>
            </div>
        `;
    });
    
    notifContent.innerHTML = html;
}

function updateNotificationBadge() {
    const badge = document.getElementById('notifBadge');
    if (!badge) {
        // Create badge if it doesn't exist
        const notifToggle = document.getElementById('notifToggle');
        const newBadge = document.createElement('div');
        newBadge.id = 'notifBadge';
        newBadge.className = 'notif-badge';
        notifToggle.parentElement.style.position = 'relative';
        notifToggle.parentElement.appendChild(newBadge);
    }
    
    const badgeElement = document.getElementById('notifBadge');
    const count = notifications.length;
    
    if (count > 0) {
        badgeElement.textContent = count > 99 ? '99+' : count;
        badgeElement.style.display = 'block';
    } else {
        badgeElement.style.display = 'none';
    }
}

function clearNotifications() {
    notifications = [];
    displayNotifications();
    updateNotificationBadge();
}

// ===== Chart Variables =====
let chartsInitialized = false;
let humidityChart, temperatureChart, combinedChart, trendChart;
const MAX_DATA_POINTS = 30;

// Data storage
let chartData = {
    labels: [],
    temperature: [],
    humidity: []
};

// ===== Chart Configuration =====
function createGradient(ctx, color1, color2) {
    const gradient = ctx.createLinearGradient(0, 0, 0, 400);
    gradient.addColorStop(0, color1);
    gradient.addColorStop(1, color2);
    return gradient;
}

function getChartColors() {
    const isDark = document.body.classList.contains('dark-mode');
    return {
        temp: {
            line: isDark ? '#ff6b9d' : '#e74c3c',
            fill: isDark ? 'rgba(255, 107, 157, 0.3)' : 'rgba(231, 76, 60, 0.3)',
            fillEnd: isDark ? 'rgba(255, 107, 157, 0.05)' : 'rgba(231, 76, 60, 0.05)'
        },
        humid: {
            line: isDark ? '#5eb3f6' : '#3498db',
            fill: isDark ? 'rgba(94, 179, 246, 0.3)' : 'rgba(52, 152, 219, 0.3)',
            fillEnd: isDark ? 'rgba(94, 179, 246, 0.05)' : 'rgba(52, 152, 219, 0.05)'
        },
        trend: {
            line: isDark ? '#7ed321' : '#27ae60',
            fill: isDark ? 'rgba(126, 211, 33, 0.3)' : 'rgba(39, 174, 96, 0.3)',
            fillEnd: isDark ? 'rgba(126, 211, 33, 0.05)' : 'rgba(39, 174, 96, 0.05)'
        },
        grid: isDark ? 'rgba(255, 255, 255, 0.1)' : 'rgba(0, 0, 0, 0.1)',
        text: isDark ? '#b8c2ff' : '#000000'
    };
}

function initCharts() {
    const colors = getChartColors();
    
    // Common chart options
    const commonOptions = {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
        interaction: {
            intersect: false,
            mode: 'index'
        },
        plugins: {
            legend: {
                display: true,
                position: 'top',
                labels: {
                    color: colors.text,
                    usePointStyle: true,
                    padding: 15
                }
            },
            tooltip: {
                backgroundColor: 'rgba(0, 0, 0, 0.8)',
                padding: 12,
                titleColor: '#fff',
                bodyColor: '#fff',
                borderColor: colors.text,
                borderWidth: 1
            }
        },
        scales: {
            x: {
                grid: {
                    color: colors.grid,
                    drawBorder: false
                },
                ticks: {
                    color: colors.text,
                    maxRotation: 0
                }
            },
            y: {
                grid: {
                    color: colors.grid,
                    drawBorder: false
                },
                ticks: {
                    color: colors.text
                },
                beginAtZero: false
            }
        }
    };

    // Humidity Chart
    const humidCtx = document.getElementById('humidityChart').getContext('2d');
    humidityChart = new Chart(humidCtx, {
        type: 'line',
        data: {
            labels: chartData.labels,
            datasets: [{
                label: 'Humidity (%)',
                data: chartData.humidity,
                borderColor: colors.humid.line,
                backgroundColor: createGradient(humidCtx, colors.humid.fill, colors.humid.fillEnd),
                borderWidth: 3,
                fill: true,
                tension: 0.4,
                pointRadius: 4,
                pointHoverRadius: 6,
                pointBackgroundColor: colors.humid.line,
                pointBorderColor: '#fff',
                pointBorderWidth: 2
            }]
        },
        options: commonOptions
    });

    // Temperature Chart
    const tempCtx = document.getElementById('temperatureChart').getContext('2d');
    temperatureChart = new Chart(tempCtx, {
        type: 'line',
        data: {
            labels: chartData.labels,
            datasets: [{
                label: 'Temperature (°C)',
                data: chartData.temperature,
                borderColor: colors.temp.line,
                backgroundColor: createGradient(tempCtx, colors.temp.fill, colors.temp.fillEnd),
                borderWidth: 3,
                fill: true,
                tension: 0.4,
                pointRadius: 4,
                pointHoverRadius: 6,
                pointBackgroundColor: colors.temp.line,
                pointBorderColor: '#fff',
                pointBorderWidth: 2
            }]
        },
        options: commonOptions
    });

    // Combined Chart
    const combinedCtx = document.getElementById('combinedChart').getContext('2d');
    combinedChart = new Chart(combinedCtx, {
        type: 'line',
        data: {
            labels: chartData.labels,
            datasets: [
                {
                    label: 'Temperature (°C)',
                    data: chartData.temperature,
                    borderColor: colors.temp.line,
                    backgroundColor: createGradient(combinedCtx, colors.temp.fill, colors.temp.fillEnd),
                    borderWidth: 3,
                    fill: true,
                    tension: 0.4,
                    pointRadius: 3,
                    pointHoverRadius: 5,
                    yAxisID: 'y'
                },
                {
                    label: 'Humidity (%)',
                    data: chartData.humidity,
                    borderColor: colors.humid.line,
                    backgroundColor: createGradient(combinedCtx, colors.humid.fill, colors.humid.fillEnd),
                    borderWidth: 3,
                    fill: true,
                    tension: 0.4,
                    pointRadius: 3,
                    pointHoverRadius: 5,
                    yAxisID: 'y1'
                }
            ]
        },
        options: {
            ...commonOptions,
            scales: {
                x: commonOptions.scales.x,
                y: {
                    type: 'linear',
                    position: 'left',
                    grid: {
                        color: colors.grid,
                        drawBorder: false
                    },
                    ticks: {
                        color: colors.temp.line
                    },
                    title: {
                        display: true,
                        text: 'Temperature (°C)',
                        color: colors.temp.line
                    }
                },
                y1: {
                    type: 'linear',
                    position: 'right',
                    grid: {
                        drawOnChartArea: false,
                        drawBorder: false
                    },
                    ticks: {
                        color: colors.humid.line
                    },
                    title: {
                        display: true,
                        text: 'Humidity (%)',
                        color: colors.humid.line
                    }
                }
            }
        }
    });

    // Trend Chart (rolling average)
    const trendCtx = document.getElementById('trendChart').getContext('2d');
    trendChart = new Chart(trendCtx, {
        type: 'line',
        data: {
            labels: chartData.labels,
            datasets: [{
                label: 'Temperature Trend',
                data: chartData.temperature,
                borderColor: colors.trend.line,
                backgroundColor: createGradient(trendCtx, colors.trend.fill, colors.trend.fillEnd),
                borderWidth: 3,
                fill: true,
                tension: 0.4,
                pointRadius: 0
            }]
        },
        options: commonOptions
    });
}

function updateCharts(temp, humid) {
    if (!chartsInitialized) return;

    const time = new Date().toLocaleTimeString();
    
    // Add new data
    chartData.labels.push(time);
    chartData.temperature.push(temp);
    chartData.humidity.push(humid);

    // Keep only last MAX_DATA_POINTS
    if (chartData.labels.length > MAX_DATA_POINTS) {
        chartData.labels.shift();
        chartData.temperature.shift();
        chartData.humidity.shift();
    }

    // Update all charts
    [humidityChart, temperatureChart, combinedChart, trendChart].forEach(chart => {
        if (chart) chart.update('none');
    });
}

function updateChartTheme() {
    if (!chartsInitialized) return;
    
    const colors = getChartColors();
    
    // Update all charts with new colors
    [humidityChart, temperatureChart, combinedChart, trendChart].forEach(chart => {
        if (chart) {
            // Update grid colors
            chart.options.scales.x.grid.color = colors.grid;
            chart.options.scales.x.ticks.color = colors.text;
            chart.options.scales.y.grid.color = colors.grid;
            chart.options.scales.y.ticks.color = colors.text;
            
            // Update legend
            chart.options.plugins.legend.labels.color = colors.text;
            
            chart.update();
        }
    });
}

// ===== Helper Functions =====
function getCurrentTime() {
    return new Date().toLocaleTimeString();
}

function updateTimeDisplay(target) {
    if (target) target.textContent = getCurrentTime();
}

function setControlsEnabled(enabled) {
    [room1OnBtn, room1OffBtn, room1AutoBtn, room2OnBtn, room2OffBtn, room2AutoBtn].forEach(btn => {
        if (btn) btn.disabled = !enabled;
    });
}

// ===== WebSocket Functions =====
function initWebSocket() {
    const gateway = `ws://${window.location.hostname}/ws`;
    try {
        websocket = new WebSocket(gateway);
    } catch (e) {
        console.error('WebSocket creation failed', e);
        scheduleReconnect();
        return;
    }

    websocket.onopen = () => {
        console.log('WebSocket connected');
        setControlsEnabled(true);
        websocket.send('getReadings');
        reconnectDelay = 2000;
    };

    websocket.onclose = () => {
        console.log('WebSocket disconnected');
        setControlsEnabled(false);
        scheduleReconnect();
    };

    websocket.onerror = (err) => {
        console.warn('WebSocket error', err);
    };

    websocket.onmessage = onMessage;
}

function scheduleReconnect() {
    if (websocket) {
        try { websocket.close(); } catch (e) {}
        websocket = null;
    }
    const delay = Math.min(reconnectDelay, maxReconnectDelay);
    console.log(`Reconnecting in ${delay} ms...`);
    setTimeout(() => {
        initWebSocket();
        reconnectDelay = Math.min(maxReconnectDelay, reconnectDelay * 2.5);
    }, delay);
}

function onMessage(event) {
    try {
        const data = JSON.parse(event.data);

        // Temperature & Humidity
        const temp = parseFloat(data.temperature ?? data.temp);
        const humid = parseFloat(data.humidity ?? data.h);

        if (!isNaN(temp)) {
            temperatureElement.textContent = temp.toFixed(1);
            updateTimeDisplay(tempTimeElement);
            // Check temperature thresholds
            checkTemperatureAlert(temp);
        }
        if (!isNaN(humid)) {
            humidityElement.textContent = humid.toFixed(1);
            updateTimeDisplay(humidTimeElement);
        }

        // Update charts if both values are valid
        if (!isNaN(temp) && !isNaN(humid)) {
            updateCharts(temp, humid);
        }

        // Sound Detection
        if (data.sound !== undefined) {
            updateTimeDisplay(soundTimeElement);
            
            if (data.sound === "detected") {
                soundBadgeElement.textContent = "Activate";
                soundBadgeElement.className = "sound-badge detected";
            } else if (data.sound === "listening") {
                soundBadgeElement.textContent = "Listening";
                soundBadgeElement.className = "sound-badge listening";
            } else if (data.sound === "quiet") {
                soundBadgeElement.textContent = "Quiet";
                soundBadgeElement.className = "sound-badge quiet";
            }
        }

        // Alert notifications from door system
        if (data.alert !== undefined) {
            if (data.alert === "Access Denied") {
                addNotification('intruder', 'INTRUDER ALERT - Access Denied!');
                // Show pop-up alert for intruder
                showIntruderAlert();
            } else if (data.alert === "Failed Attempt") {
                addNotification('failed', 'Failed door access attempt detected');
            }
        }

        // Room 1 State
        if (data.room1 !== undefined) {
            const state = String(data.room1);
            room1StateTxt.textContent = state;
            
            if (state === "ON") {
                light1State.textContent = "Active";
                light1State.className = "state-box active";
            } else {
                light1State.textContent = "Inactive";
                light1State.className = "state-box inactive";
            }
        }

        // Room 2 State
        if (data.room2 !== undefined) {
            const state = String(data.room2);
            room2StateTxt.textContent = state;
            
            if (state === "ON") {
                light2State.textContent = "Active";
                light2State.className = "state-box active";
            } else {
                light2State.textContent = "Inactive";
                light2State.className = "state-box inactive";
            }
        }

    } catch (err) {
        console.error("WS Parse Error:", err);
    }
}

// Track last temperature notification to prevent spam
let lastTempNotif = { level: '', time: 0 };
const TEMP_NOTIF_COOLDOWN = 300000; // 5 minutes between same-level notifications

function checkTemperatureAlert(temp) {
    const now = Date.now();
    let level = '';
    let message = '';
    let notifType = 'heat';
    
    if (temp >= 52) {
        level = 'extreme-danger';
        message = `🔥 EXTREME DANGER! Temperature: ${temp.toFixed(1)}°C - Take immediate action!`;
        notifType = 'heat-extreme';
    } else if (temp >= 42) {
        level = 'danger';
        message = `⚠️ DANGER! Temperature: ${temp.toFixed(1)}°C - Heat stroke likely!`;
        notifType = 'heat-danger';
    } else if (temp >= 33) {
        level = 'extreme-caution';
        message = `🌡️ Extreme Caution: Temperature ${temp.toFixed(1)}°C - Heat exhaustion possible`;
        notifType = 'heat-caution';
    } else if (temp >= 27) {
        level = 'caution';
        message = `💧 Caution: Temperature ${temp.toFixed(1)}°C - Stay hydrated`;
        notifType = 'heat';
    }
    
    // Only notify if level changed or cooldown passed
    if (level && (lastTempNotif.level !== level || now - lastTempNotif.time > TEMP_NOTIF_COOLDOWN)) {
        addNotification(notifType, message);
        lastTempNotif = { level: level, time: now };
    }
}

// Show intruder alert pop-up
function showIntruderAlert() {
    const alertDiv = document.createElement('div');
    alertDiv.className = 'intruder-alert-popup';
    alertDiv.innerHTML = `
        <div class="intruder-alert-content">
            <div class="intruder-alert-icon">🚨</div>
            <div class="intruder-alert-title">INTRUDER DETECTED!</div>
            <div class="intruder-alert-message">Unauthorized access attempt blocked</div>
            <button class="intruder-alert-btn" onclick="closeIntruderAlert()">OK</button>
        </div>
    `;
    document.body.appendChild(alertDiv);
    
    // Auto-close after 10 seconds
    setTimeout(() => {
        closeIntruderAlert();
    }, 10000);
}

function closeIntruderAlert() {
    const alert = document.querySelector('.intruder-alert-popup');
    if (alert) {
        alert.remove();
    }
}

// ===== Room Control Functions =====
function sendRoom1Command(cmd) {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
        websocket.send("room1:" + cmd);
    } else {
        console.warn('WebSocket not ready - ignoring command');
    }
}

function sendRoom2Command(cmd) {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
        websocket.send("room2:" + cmd);
    } else {
        console.warn('WebSocket not ready - ignoring command');
    }
}

// ===== Room Control Event Listeners =====
room1OnBtn.addEventListener("click", () => sendRoom1Command("ON"));
room1OffBtn.addEventListener("click", () => sendRoom1Command("OFF"));
room1AutoBtn.addEventListener("click", () => sendRoom1Command("AUTO"));

room2OnBtn.addEventListener("click", () => sendRoom2Command("ON"));
room2OffBtn.addEventListener("click", () => sendRoom2Command("OFF"));
room2AutoBtn.addEventListener("click", () => sendRoom2Command("AUTO"));

// Add clear notifications button
document.getElementById("clearNotif")?.addEventListener("click", clearNotifications);

// ===== Initialization =====
window.addEventListener("load", () => {
    setControlsEnabled(false);
    initWebSocket();
    displayNotifications();
});
