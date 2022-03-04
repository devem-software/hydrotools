// Get a RTDB instance
import { initializeApp } from 'firebase/app'
import {
  getDatabase
} from 'firebase/database'

export const fb = initializeApp({
  apiKey: 'AIzaSyC0v-XlIcscm_BlAKdyYxHPzp5CQpmxBgw',
  authDomain: 'hidrologia-u-distrital.firebaseapp.com',
  databaseURL: 'https://hidrologia-u-distrital-default-rtdb.firebaseio.com',
  projectId: 'hidrologia-u-distrital',
  storageBucket: 'hidrologia-u-distrital.appspot.com',
  messagingSenderId: '248164432159',
  appId: '1:248164432159:web:01ecba244de313095e182a',
  measurementId: 'G-QWMK1F78QL'
})

export const db = getDatabase()
