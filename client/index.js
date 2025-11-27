const main = async () => {
	const localVideo = document.getElementById('localVideo');
	const remoteVideo = document.getElementById('remoteVideo');
	const startButton = document.getElementById('startButton');

	const socket = new WebSocket('ws://localhost:9000');
	socket.addEventListener('error', console.error);

	const configuration = {
		iceServers: [
			{
				urls: 'stun:stun.l.google.com:19302',
			},
		],
	};
	const peerConnection = new RTCPeerConnection(configuration);

	try {
		const constraints = {
			audio: true,
			video: {
				width: 640,
				height: 480,
			},
		};

		const localStream = await navigator.mediaDevices.getUserMedia(constraints);
		localVideo.srcObject = localStream;
		localStream.getTracks().forEach((track) => peerConnection.addTrack(track, localStream));
	} catch (error) {
		console.error('Media error', error);
	}

	peerConnection.onconnectionstatechange = () => {
		if (peerConnection.connectionState) {
			console.log(peerConnection.connectionState);
		}
	};

	peerConnection.onicecandidate = (event) => {
		const candidate = event.candidate;
		if (candidate) {
			console.log('[OUT]', candidate);
			socket.send(JSON.stringify({ candidate }));
		}
	};

	peerConnection.ontrack = (event) => {
		console.log('Add track', event.streams[0].getTracks());
		remoteVideo.srcObject = event.streams[0];
	};

	socket.addEventListener('message', async (event) => {
		const message = JSON.parse(event.data);
		console.log('[IN]', message);

		if (message.offer) {
			try {
				const offer = new RTCSessionDescription(message.offer);
				peerConnection.setRemoteDescription(offer);

				const answer = await peerConnection.createAnswer();
				await peerConnection.setLocalDescription(answer);

				console.log('[OUT]', answer);
				socket.send(JSON.stringify({ answer }));
			} catch (error) {
				console.error('Offer error', error);
			}
		}

		if (message.answer) {
			try {
				const answer = new RTCSessionDescription(message.answer);
				await peerConnection.setRemoteDescription(answer);
			} catch (error) {
				console.error('Answer error', error);
			}
		}

		if (message.candidate) {
			try {
				const candidate = new RTCIceCandidate(message.candidate);
				await peerConnection.addIceCandidate(candidate);
			} catch (error) {
				console.error('Candidate error', error);
			}
		}
	});

	startButton.addEventListener('click', async () => {
		const offer = await peerConnection.createOffer();
		await peerConnection.setLocalDescription(offer);

		console.log('[OUT]', offer);
		socket.send(JSON.stringify({ offer }));
	});
};

main();
