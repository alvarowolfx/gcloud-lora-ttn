import React from 'react'
import { BrowserRouter as Router, Route, Redirect, Switch } from 'react-router-dom'

import Navbar from './components/Navbar'

import DeviceList from './DeviceList'
import DeviceDetails from './DeviceDetails'

function App() {
  return (
    <Router>
      <Navbar />
      <Switch>
        <Route exact path="/devices" component={DeviceList} />
        <Route exact path="/devices/:id" component={DeviceDetails} />
        <Redirect to="/devices" />
      </Switch>
    </Router>
  )
}

export default App