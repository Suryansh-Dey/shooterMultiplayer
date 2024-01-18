class Games {
    constructor() {
        this.nextGameId = 1
        this.playerOnQueue = 0
        this.games = {}
    }
    joinRandom() {
        let id = this.playerOnQueue + this.nextGameId.toString()
        if (this.playerOnQueue) {
            this.games[this.nextGameId] = ["NULL", "NULL"]
            this.playerOnQueue = 0
            this.nextGameId += 1
        }
        else this.playerOnQueue = 1
        return id
    }
    generateCode() {
        this.nextGameId++
        return '0' + (this.nextGameId - 1).toString()
    }
    joinByCode(player1Code) {
        let gameID = player1Code.slice(1)
        if (this.games.hasOwnProperty(gameID) || parseInt(gameID) >= this.nextGameId)
            return '0'
        this.games[gameID] = ["NULL", "NULL"]
        return '0' + gameID
    }
    fetch(id) {
        let gameID = id.slice(1)
        if (!this.games.hasOwnProperty(gameID))
            return '0'
        if (id[0] == '0')
            return this.games[gameID][1]
        return this.games[gameID][0]
    }
    update(id, state) {
        let gameID = id.slice(1)
        if (!this.games.hasOwnProperty(gameID))
            return
        this.games[gameID][parseInt(id[0])] = state
    }
    isAvailable(id) {
        return this.games.hasOwnProperty(id.slice(1))
    }
    quit(id) {
        delete this.games[id.slice(1)]
    }
}

module.exports = Games