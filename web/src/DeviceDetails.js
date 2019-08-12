import React, { useState, useEffect } from 'react'

import firebase from 'firebase/app'
import { format, differenceInHours } from 'date-fns'
import { LineChart, Line, CartesianGrid, ResponsiveContainer, XAxis, YAxis, Tooltip, Legend } from 'recharts'
import Grid from '@material-ui/core/Grid'
import ButtonGroup from '@material-ui/core/ButtonGroup'
import Button from '@material-ui/core/Button'
import List from '@material-ui/core/List'
import Paper from '@material-ui/core/Paper'
import ListItem from '@material-ui/core/ListItem'
import ListItemIcon from '@material-ui/core/ListItemIcon'
import ListSubheader from '@material-ui/core/ListSubheader'
import ListItemText from '@material-ui/core/ListItemText'
import Typography from '@material-ui/core/Typography'

import { withStyles } from '@material-ui/core/styles'
import {
  Timer,
  Wifi as SensorIcon,
} from '@material-ui/icons'

import config from './config'

const historyUrl = config.baseUrl + "/HandleDeviceHistoryQuery"

const styles = {
  root: {
    flexGrow: 1,
  },
  grid: {
    marginTop: 8,
  },
}

function DeviceInformation({ device }) {
  return (
    <Paper elevation={2}>
      <List
        subheader={
          <ListSubheader component="div" id="nested-list-subheader">
            Device Information
          </ListSubheader>
        }>
        <ListItem>
          <ListItemIcon>
            <SensorIcon />
          </ListItemIcon>
          <ListItemText
            primary="Device ID"
            secondary={device.deviceId} />
        </ListItem>
        <ListItem>
          <ListItemIcon>
            <SensorIcon />
          </ListItemIcon>
          <ListItemText
            primary="Serial"
            secondary={device.serial} />
        </ListItem>
        <ListItem>
          <ListItemIcon>
            <Timer />
          </ListItemIcon>
          <ListItemText
            primary="Last update"
            secondary={format(device.meta.updated, "DD/MM/YYYY HH:mm:ss")} />
        </ListItem>
      </List>
    </Paper>
  )
}

function DeviceChannelInformation({ device, channel, history }) {
  const [period, setPeriod] = useState("hour")
  const diffs = {
    hour: 1,
    day: 24,
    week: 24 * 7
  }
  const channelData = device && device.data && device.data[channel]
  const types = Object.keys(channelData)
  const data = history
    .map(point => {
      return {
        ...point.data[channel],
        time: new Date(point.time).getTime()
      }
    }).filter(point => {
      const diff = Math.abs(differenceInHours(point.time, new Date()))
      const currentDiff = diffs[period]
      return diff <= currentDiff
    })

  return (
    (
      <Paper elevation={2} >
        <List
          subheader={
            <ListSubheader component="div" id="nested-list-subheader">
              Channel {channel}
            </ListSubheader>
          }>
          {types.map(type => {
            return (
              <ListItem key={type}>
                <ListItemIcon>
                  <SensorIcon />
                </ListItemIcon>
                <ListItemText
                  primary={`Current Value: ${channelData[type]}`}
                  secondary={`Channel type: ${type}`} />
              </ListItem>
            )
          })}
          {data.length > 0 && <ListItem style={{ width: '100%', height: 300, flexDirection: 'column' }}>
            <ButtonGroup size="small" aria-label="small outlined button group">
              <Button color={period === 'hour' && "primary"} onClick={() => setPeriod('hour')}>Hour</Button>
              <Button color={period === 'day' && "primary"} onClick={() => setPeriod('day')}>Day</Button>
              <Button color={period === 'week' && "primary"} onClick={() => setPeriod('week')}>Week</Button>
            </ButtonGroup>
            <ResponsiveContainer>
              <LineChart data={data}>
                {types.map(type => (
                  <Line key={type}
                    type="monotone"
                    strokeWidth={2}
                    dataKey={type}
                    stroke="#8884d8" />
                ))}
                <XAxis
                  dataKey="time"
                  domain={['auto', 'auto']}
                  tickFormatter={timeStr => format(timeStr, 'DD/MM HH:mm')}
                  type="number" />
                <YAxis name="Value" />
                <CartesianGrid strokeDasharray="3 3" />
                <Tooltip labelFormatter={value => {
                  return format(value, 'DD/MM HH:mm')
                }} />
                <Legend />
              </LineChart>
            </ResponsiveContainer>
          </ListItem>}
        </List>
      </Paper>
    )
  )
}

function DeviceDetails({ classes, match }) {
  const [device, setDevice] = useState(null)
  const [deviceHistory, setDeviceHistory] = useState([])
  const [isLoading, setLoading] = useState(false)

  const deviceId = match.params.id

  useEffect(() => {
    const db = firebase.database()
    console.log('device', deviceId)
    const deviceRef = db.ref('devices').child(deviceId)

    const handleUpdate = (snap) => {
      const device = snap.val()
      setDevice(device)
      setLoading(false)
    }

    const url = historyUrl + '?deviceId=' + deviceId
    const fetchHistoryData = () => {
      fetch(url)
        .then(res => res.json())
        .then(json => {
          const results = json.results.map(r => {
            return {
              data: JSON.parse(r.data),
              time: r.time,
            }
          })
          console.log('results', results)
          setDeviceHistory(results)
        })
    }

    setLoading(true)

    deviceRef.on('value', handleUpdate)
    fetchHistoryData()

    return () => {
      deviceRef.off('value', handleUpdate)
    }
  }, [])

  const channels = device && device.data && Object.keys(device.data)

  return (
    <div className={classes.root}>
      <Grid container className={classes.grid} spacing={1}>
        {isLoading && (
          <Typography variant="h6" color="inherit" style={{ margin: 16 }}>
            <br /> Loading ...
        </Typography>
        )}
        {device && (
          <Grid item xs={12} md={12}>
            <DeviceInformation device={device} />
          </Grid>
        )}
        {channels && channels.map(channel => {
          return (
            <Grid item xs={12} md={6} key={channel}>
              <DeviceChannelInformation
                device={device}
                channel={channel}
                history={deviceHistory} />
            </Grid>
          )
        })}
      </Grid>
    </div>
  )
}

export default withStyles(styles)(DeviceDetails)