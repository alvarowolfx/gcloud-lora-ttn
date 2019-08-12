import React from 'react'
import PropTypes from 'prop-types'
import { withRouter } from 'react-router'
import { withStyles } from '@material-ui/core/styles'
import AppBar from '@material-ui/core/AppBar'
import Link from '@material-ui/core/Link'
import Toolbar from '@material-ui/core/Toolbar'
import Paper from '@material-ui/core/Paper'
import Breadcrumbs from '@material-ui/core/Breadcrumbs'
import Typography from '@material-ui/core/Typography'
import NavigateNextIcon from '@material-ui/icons/NavigateNext'

const styles = {
  root: {
    justifyContent: 'center',
    flexWrap: 'wrap',
  },
  breadcrumb: {
    margin: 16,
    marginTop: 80,
  }
}

function Navbar(props) {
  const { classes, location, history } = props
  const path = location.pathname
  const params = /\/devices\/(.*)/.exec(path)
  let deviceId
  if (params) {
    deviceId = params[1]
  }
  return (
    <div className={classes.root}>
      <AppBar position="fixed" color="primary">
        <Toolbar>
          <Typography variant="h6" color="inherit">
            The Things Network Dashboard
          </Typography>
        </Toolbar>
      </AppBar>
      <Paper elevation={0} className={classes.breadcrumb}>
        {deviceId && <Breadcrumbs separator={<NavigateNextIcon fontSize="small" />} aria-label="breadcrumb">
          <Link color="inherit" href="/devices" onClick={e => {
            e.preventDefault()
            history.goBack()
          }}>
            Home
          </Link>
          <Link color="inherit">
            Device {deviceId}
          </Link>
        </Breadcrumbs>}
      </Paper>
    </div>
  )
}

Navbar.propTypes = {
  classes: PropTypes.object.isRequired,
}

export default withRouter(withStyles(styles)(Navbar))