const pieceChars = {
    'K': '♔', 'Q': '♕', 'R': '♖', 'B': '♗', 'N': '♘', 'P': '♙',
    'k': '♚', 'q': '♛', 'r': '♜', 'b': '♝', 'n': '♞', 'p': '♟'
};

let boardState = [];
let selectedSquare = null;
let currentTurn = 'w';
let timerInterval = null;
let whiteTime = 0;
let blackTime = 0;
let gameOver = false;
let selectedMode = 'pvp';
let aiThinking = false;
let gameStarted = false;

function initBoard() {
    const boardDiv = document.getElementById('board');
    boardDiv.innerHTML = '';
    for (let r = 0; r < 8; r++) {
        for (let c = 0; c < 8; c++) {
            const square = document.createElement('div');
            square.className = `square ${(r + c) % 2 === 0 ? 'light-square' : 'dark-square'}`;
            square.id = `sq-${r}-${c}`;
            square.onclick = () => handleSquareClick(r, c);
            boardDiv.appendChild(square);
        }
    }
}

async function sendCmd(cmd, args = []) {
    const res = await fetch('/api/', {
        method: 'POST',
        body: JSON.stringify({ cmd, args })
    });
    return await res.json();
}

function updateUI(state) {
    if (state.error) {
        showToast(state.error);
        return;
    }
    
    document.lastFen = state.fen;
    // Parse FEN
    const fenParts = state.fen.split(' ');
    const boardRows = fenParts[0].split('/');
    
    for (let r = 0; r < 8; r++) {
        let c = 0;
        for (let char of boardRows[r]) {
            if (!isNaN(char)) {
                let emptySpaces = parseInt(char);
                for (let i = 0; i < emptySpaces; i++) {
                    document.getElementById(`sq-${r}-${c}`).innerText = '';
                    document.getElementById(`sq-${r}-${c}`).style.color = '';
                    c++;
                }
            } else {
                let sq = document.getElementById(`sq-${r}-${c}`);
                sq.innerText = pieceChars[char];
                sq.style.color = char === char.toUpperCase() ? '#fff' : '#000';
                if(char === char.toUpperCase()) {
                    sq.style.textShadow = '0px 0px 3px #000';
                } else {
                    sq.style.textShadow = '0px 0px 3px #fff';
                }
                c++;
            }
        }
    }
    
    currentTurn = state.turn;
    whiteTime = state.white_time;
    blackTime = state.black_time;
    gameOver = state.status !== 'active' && state.status !== 'check';
    
    let statusText = state.status === 'waiting' ? 'Choose Mode & Start' :
                     state.status === 'check' ? 'CHECK!' : 
                     state.status === 'white_won_mate' ? 'White Wins by Checkmate!' :
                     state.status === 'black_won_mate' ? 'Black Wins by Checkmate!' :
                     state.status === 'draw_stalemate' ? 'Draw by Stalemate!' :
                     state.status === 'white_won_time' ? 'White Wins on Time!' :
                     state.status === 'black_won_time' ? 'Black Wins on Time!' :
                     currentTurn === 'w' ? "White's Turn" : "Black's Turn";
                     
    document.getElementById('status-plank').innerText = statusText;
    
    document.getElementById('timer-w').classList.toggle('active-turn', currentTurn === 'w' && !gameOver);
    document.getElementById('timer-b').classList.toggle('active-turn', currentTurn === 'b' && !gameOver);
    
    updateTimersDisplay();
    
    for(let r=0; r<8; r++) {
        for(let c=0; c<8; c++) {
            document.getElementById(`sq-${r}-${c}`).classList.remove('selected');
        }
    }
}

async function handleSquareClick(r, c) {
    if (!gameStarted) {
        showToast("Click START GAME first!");
        return;
    }
    if (gameOver || aiThinking) return;
    
    if (!selectedSquare) {
        const piece = document.getElementById(`sq-${r}-${c}`).innerText;
        if (piece) { 
            selectedSquare = { r, c };
            document.getElementById(`sq-${r}-${c}`).classList.add('selected');
        }
    } else {
        const fromAlg = String.fromCharCode('a'.charCodeAt(0) + selectedSquare.c) + (8 - selectedSquare.r);
        const toAlg = String.fromCharCode('a'.charCodeAt(0) + c) + (8 - r);
        
        selectedSquare = null;
        let moveStr = fromAlg + toAlg;
        
        if ((moveStr[1] === '7' && moveStr[3] === '8') || (moveStr[1] === '2' && moveStr[3] === '1')) {
            moveStr += 'q';
        }
        
        let res = await sendCmd('move', [moveStr]);
        
        if (res.fen === document.lastFen) {
            showToast("Invalid Move!");
            for(let r=0; r<8; r++) {
                for(let c=0; c<8; c++) {
                    document.getElementById(`sq-${r}-${c}`).classList.remove('selected');
                }
            }
            return;
        }
        
        document.lastFen = res.fen;
        updateUI(res);
        
        if (res.status === 'active' || res.status === 'check') {
            if (res.turn === 'b' && selectedMode === 'pvai') {
                aiThinking = true;
                document.getElementById('ai-overlay').classList.remove('hidden');
                setTimeout(async () => {
                    let aiRes = await sendCmd('aimove');
                    aiThinking = false;
                    document.getElementById('ai-overlay').classList.add('hidden');
                    updateUI(aiRes);
                }, 100);
            }
        }
    }
}

function selectMode(mode) {
    selectedMode = mode;
    document.getElementById('btn-pvp').classList.remove('selected');
    document.getElementById('btn-pvai').classList.remove('selected');
    document.getElementById(`btn-${mode}`).classList.add('selected');
}

async function startGame() {
    gameStarted = true;
    const res = await sendCmd('init', [selectedMode, '10']); // 10 minutes
    updateUI(res);
    startTimer();
}

function restartGame() {
    gameStarted = false;
    if (timerInterval) clearInterval(timerInterval);
    updateUI({
        fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        turn: "w",
        white_time: 600000,
        black_time: 600000,
        status: "waiting"
    });
    document.getElementById('timer-w').classList.remove('active-turn');
    document.getElementById('timer-b').classList.remove('active-turn');
}

function showToast(msg) {
    const toast = document.getElementById('toast');
    toast.innerText = msg;
    toast.classList.remove('hidden');
    setTimeout(() => { toast.classList.add('hidden'); }, 2000);
}

async function undoMove() {
    const res = await sendCmd('undo');
    updateUI(res);
}

async function redoMove() {
    const res = await sendCmd('redo');
    updateUI(res);
}

function updateTimersDisplay() {
    const formatTime = (seconds) => {
        const m = Math.floor(seconds / 60);
        const s = seconds % 60;
        return `${m}:${s < 10 ? '0' : ''}${s}`;
    };
    document.getElementById('white-timer').innerText = formatTime(whiteTime);
    document.getElementById('black-timer').innerText = formatTime(blackTime);
}

function startTimer() {
    if (timerInterval) clearInterval(timerInterval);
    timerInterval = setInterval(async () => {
        if (!gameOver) {
            const res = await sendCmd('state');
            updateUI(res);
        }
    }, 1000);
}

initBoard();
restartGame();
