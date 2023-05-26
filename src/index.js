// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyDCZAR5sLXy_H2bwkwSAJ0g2lgiR0MuwlI",
  authDomain: "ababonbabantocs2.firebaseapp.com",
  projectId: "ababonbabantocs2",
  storageBucket: "ababonbabantocs2.appspot.com",
  messagingSenderId: "404747337011",
  appId: "1:404747337011:web:767c832524de2971901246",
  measurementId: "G-KSWRBVNXQW"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);