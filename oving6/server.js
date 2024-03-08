const net = require('net');
const crypto = require('crypto');
const fs = require('fs'); // Inkluder fs-modulen for å skrive til loggfil

const clients = []; // Holder styr på tilknyttede klienter

// Funksjon for å logge til en fil
const logFilePath = './server.log'; // Angi plasseringen av loggfilen

function logToFile(message) {
    const timestamp = new Date().toISOString();
    fs.appendFile(logFilePath, `${timestamp} - ${message}\n`, (err) => {
        if (err) {
            console.error('Error writing to log file:', err);
        }
    });
}

function encodeWebSocketMessage(message) {
    const messageBytes = Buffer.from(message);
    const length = messageBytes.length;
    const buffer = Buffer.alloc(2 + length); // Enkel ramme: 2 byte header + meldingslengde
    buffer[0] = 0x81; // FIN bit + opcode for tekst
    buffer[1] = length; // antar at meldingen er mindre enn 126 tegn lang
    messageBytes.copy(buffer, 2);
    return buffer;
}

// Enkel funksjon for å dekode innkommende WebSocket-meldinger
function decodeWebSocketMessage(data) {
    // Denne implementasjonen er veldig grunnleggende og antar meldingen er kort
    if (data.length < 6) return ''; // Minimal sjekk for å unngå feil
    const mask = data.slice(2, 6);
    const payload = data.slice(6);
    let decoded = "";
    for (let i = 0; i < payload.length; i++) {
        decoded += String.fromCharCode(payload[i] ^ mask[i % 4]);
    }
    return decoded;
}

// WebSocket-server
const wsServer = net.createServer((connection) => {
    console.log('Client connected');
    logToFile('Client connected');

    connection.on('data', (data) => {
        if (data.toString().includes('GET /')) {
            console.log('Handshake request received from client');
            logToFile('Handshake request received from client');

            const key = data.toString().match(/Sec-WebSocket-Key: (.+)/)[1].trim();
            const acceptValue = crypto.createHash('sha1').update(key + '258EAFA5-E914-47DA-95CA-C5AB0DC85B11', 'binary').digest('base64');
            const responseHeaders = [
                'HTTP/1.1 101 Web Socket Protocol Handshake',
                'Upgrade: WebSocket',
                'Connection: Upgrade',
                `Sec-WebSocket-Accept: ${acceptValue}`,
            ];
            connection.write(responseHeaders.join('\r\n') + '\r\n\r\n');

            console.log('Handshake response sent to client');
            logToFile('Handshake response sent to client');
        } else {
            const decodedMessage = decodeWebSocketMessage(data);
            console.log('Message from client:', decodedMessage);
            logToFile('Message received from client');

            clients.forEach(client => {
                if (client !== connection) { // Valgfritt: Unngå å sende meldingen tilbake til avsenderen
                    client.write(encodeWebSocketMessage(decodedMessage));
                }
            });
        }
    });

    connection.on('end', () => {
        console.log('Client disconnected');
        logToFile('Client disconnected');
        const index = clients.indexOf(connection);
        if (index !== -1) {
            clients.splice(index, 1);
        }
    });

    clients.push(connection);
});

wsServer.on('error', (error) => {
    console.error('Error:', error);
    logToFile(`Server error: ${error.message}`);
});

wsServer.listen(3001, () => {
    console.log('WebSocket server listening on port 3001');
    logToFile('WebSocket server started on port 3001');
});
