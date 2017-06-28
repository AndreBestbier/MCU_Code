// Calculate the mean difference of the previous n samples
int MAX30100_meanDiff(int M, int n)
{
  sum -= LM[index];
  LM[index] = M;
  sum += LM[index];
  index++;
  index = index % LM_SIZE;
  if (count < LM_SIZE) count++;

  avg = sum / count;
  return avg - M;
}

void MAX30100_DCRemover ()
{
  float alphaRed = 0.95;
  float alphaIR = 0.95;

  float red_new_w  = rawRedValue + alphaRed * red_w;
  redAC = red_new_w - red_w;
  redDC = rawRedValue - redAC;
  red_w = red_new_w;

  float ir_new_w  = rawIRValue + alphaRed * ir_w;
  irAC = ir_new_w - ir_w;
  irDC = rawIRValue - irAC;
  ir_w = ir_new_w;

  //Serial.print(rawIRValue);
  //Serial.print(" ");
  //Serial.println(irAC);
}

uint8_t MAX30100_CurrentBalancing(int redLedCurrent)
{
  // Follower that adjusts the red led current in order to have comparable DC baselines between
  // red and IR leds. The numbers are really magic: the less possible to avoid oscillations

  //Serial.print("checkCurrentBias function current: ");  Serial.println(redLedCurrent);

  if (millis() - lastBiasCheck > CURRENT_ADJUSTMENT_PERIOD_MS)
  {
    //Serial.println("***checkCurrentBias***");
    bool changed = false;

    if (irDC - redDC > 4000 && redLedCurrent < i50)
    {
      //Serial.println("Current: ");  Serial.println(redLedCurrent);
      redLedCurrent = redLedCurrent + 1;
      changed = true;
      //Serial.println("Increase red current");
    }
    else if (irDC - redDC > 4000 && redLedCurrent == i50)
    {
      irLedCurrent = irLedCurrent - 1;
      changed = true;
      //Serial.println("Decrease IR current");
    }
    else if (redDC - irDC > 4000 && redLedCurrent > 0)
    {
      //Serial.println("Current: ");  Serial.println(redLedCurrent);
      redLedCurrent = redLedCurrent - 1;
      changed = true;
      //Serial.println("Decrease red current");
    }

    if (changed)
    {
      //Update the current
      MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_LED_CONFIG, (redLedCurrent << 4) | irLedCurrent); // 4 bits for RED current and 4 bits for IR current

      //Serial.print("Current changed to: "); Serial.println(redLedCurrent);
      //Serial.print("LED Config:         ");
      //Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_LED_CONFIG),  BIN);

    }
    lastBiasCheck = millis();
  }
  return redLedCurrent;
  //Serial.print("Returned current at the end of the function: "); Serial.println(redLedCurrent);
}

void MAX30100_SpO2Calculator(int irAC, int redAC)
{
  //  Serial.print(irAC);
  //  Serial.print(",");
  //  Serial.println(redAC);

  irACsqrSum += irAC * irAC;
  redACsqrSum += redAC * redAC;

  //Serial.print("irACsqrSum = "); Serial.println(irACsqrSum);
  //Serial.print("redACsqrSum = "); Serial.println(redACsqrSum);

  acSqrSum_Counter++;

  if (acSqrSum_Counter == SAMPLES_IN_SPO2_CALCULATION)
  {
    //Serial.print("irACsqrSum = "); Serial.println(irACsqrSum);
    //Serial.print("redACsqrSum = "); Serial.println(redACsqrSum);

    //Serial.print("Log of 150 red samples = "); Serial.println(log(redACsqrSum / acSqrSum_Counter));
    //Serial.print("Log of 150 ir samples = "); Serial.println(log(irACsqrSum / acSqrSum_Counter));

    r_SpO2 =  log(redACsqrSum / acSqrSum_Counter) / log(irACsqrSum / acSqrSum_Counter);
    //Serial.print("SpO2 calculation ratio = "); Serial.println(r_SpO2);

    irACsqrSum = 0;
    redACsqrSum = 0;
    acSqrSum_Counter = 0;
  }
}


void beatDetection()
{

}




















