const ws = require('ws');

const colors = {
	reset: '\x1b[0m',
	red: '\x1b[31m',
	green: '\x1b[32m',
	yellow: '\x1b[33m',
	cyan: '\x1b[36m',
};

const log = {
	info: (...msg) => {
		console.log(colors.cyan, '[INFO] ', colors.reset, ...msg);
	},
	success: (...msg) => {
		console.log(colors.green, '[OK]   ', colors.reset, ...msg);
	},
	warning: (...msg) => {
		console.log(colors.yellow, '[WARNING] ', colors.reset, ...msg);
	},
	error: (...msg) => {
		console.log(colors.red, '[ERROR]', colors.reset, ...msg);
	},
};

const handleConnection = (socket, request) => {
	log.success('Client connected');

	socket.on('close', handleClose(socket));
	socket.on('message', handleMessage(socket));
	socket.on('error', handleError(socket));
};

const handleClose = (socket) => (code, reason) => {
	log.warning('Client disconnected');
};

const handleMessage = (socket) => (data, isBinary) => {
	const payload = JSON.parse(data);

	if (payload.offer || payload.answer || payload.candidate) {
		log.info(payload);

		// Forward offer/answer/candidate to other clients.
		server.clients.forEach((client) => {
			if (client !== socket && client.readyState === socket.OPEN) {
				client.send(JSON.stringify(payload));
			}
		});
	} else {
		log.error(payload);
	}
};

const handleError = (socket) => (error) => {
	log.error(error.message || error || 'Unknown error');
	socket.close();
};

const server = new ws.WebSocketServer({
	port: 9000,
});

server.on('connection', handleConnection);

console.log(`🚀 Service running on ws://localhost:${server.options.port}`);
