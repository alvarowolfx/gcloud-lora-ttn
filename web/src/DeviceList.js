import React, { useState, useEffect } from 'react'

import firebase from 'firebase/app'
import { format } from 'date-fns'
import Grid from '@material-ui/core/Grid'
import List from '@material-ui/core/List'
import Paper from '@material-ui/core/Paper'
import ListItem from '@material-ui/core/ListItem'
import ListItemIcon from '@material-ui/core/ListItemIcon'
import ListItemText from '@material-ui/core/ListItemText'
import Typography from '@material-ui/core/Typography'

import { withStyles } from '@material-ui/core/styles'
import {
  Wifi as SensorIcon,
  ChevronRightSharp as ArrowRight
} from '@material-ui/icons'

const styles = {
  grid: {
    margin: 8
  },
}


function DeviceList({ classes, history }) {
  const [devices, setDevices] = useState({})
  const [isLoading, setLoading] = useState(false)

  useEffect(() => {
    const db = firebase.database()
    const devicesRef = db.ref('devices')

    const handleUpdate = (snap) => {
      const devices = snap.val()
      setDevices(devices)
      setLoading(false)
    }

    setLoading(true)

    devicesRef.on('value', handleUpdate)

    return () => {
      devicesRef.off('value', handleUpdate)
    }
  }, [])

  const navigateToDevice = (device) => {
    history.push(`/devices/${device.deviceId}`)
  }

  return (
    <Grid item xs={12} md={12} className={classes.grid}>
      {isLoading && (
        <Typography variant="h6" color="inherit" style={{ margin: 16 }}>
          <br /> Loading Devices ...
        </Typography>
      )}
      <List>
        {Object.values(devices).map(device => {
          return (
            <Paper key={device.deviceId}
              elevation={1}
              onClick={() => navigateToDevice(device)}>
              <ListItem>
                <ListItemIcon>
                  <SensorIcon />
                </ListItemIcon>
                <ListItemText
                  primary={device.deviceId}
                  secondary={format(device.meta.updated, "DD/MM/YYYY HH:mm:ss")} />
                <ListItemIcon>
                  <ArrowRight />
                </ListItemIcon>
              </ListItem>
            </Paper>
          )
        })}
      </List>
    </Grid>
  )
}

export default withStyles(styles)(DeviceList)