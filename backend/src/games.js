class Games {
    constructor() {
        this.nextGameId = 1
        this.playerOnQueue = 0
        this.games = {}
    }
    joinRandom() {
        if (this.gameOnQueue) {
            this.games[this.nextGameId] = ["NULL", "NULL"]
            this.gameOnQueue = 0
            this.nextGameId += 1
            return (this.nextGameId - 1).toString() + '1'
        }
        else {
            this.gameOnQueue = this.nextGameId.toString()
            return this.nextGameId.toString() + '0'
        }
    }
    generateCode() {
        this.nextGameId++
        return (this.nextGameId - 1).toString() + '0'
    }
    joinByCode(player1Code) {
        let gameID = player1Code.slice(0, -1)
        if (this.games.hasOwnProperty(gameID) || parseInt(gameID) >= this.nextGameId)
            return '0'
        this.games[gameID] = ["NULL", "NULL"]
        return gameID + '0'
    }
    fetch(id) {
        let gameID = id.slice(0, -1)
        if (!this.games.hasOwnProperty(gameID))
            return '0'
        if (id[id.length - 1] == '0')
            return this.games[gameID][1]
        return this.games[gameID][0]
    }
    update(id, state) {
        let gameID = id.slice(0, -1)
        if (!this.games.hasOwnProperty(gameID))
            return
        this.games[gameID][parseInt(id[id.length - 1])] = state
    }
    isAvailable(id) {
        return this.games.hasOwnProperty(id.slice(0, -1))
    }
    quit(id) {
        let gameID = id.slice(0, -1)
        if (this.gameOnQueue == gameID)
            this.gameOnQueue = 0
        else
            delete this.games[gameID]
    }
}

module.exports = Games