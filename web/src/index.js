import React from 'react'
import ReactDOM from 'react-dom'
import firebase from 'firebase/app'
import "firebase/database"

import config from './config'
import App from './App'

// Initialize Firebase
firebase.initializeApp(config.fbConfig)

ReactDOM.render(<App />, document.querySelector('#app'))