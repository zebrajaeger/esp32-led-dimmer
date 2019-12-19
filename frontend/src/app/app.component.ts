import {Component, OnInit, ViewChildren} from '@angular/core';
import {SliderData} from './channel-slider/channel-slider.data';
import {MatSlider, MatSliderChange} from '@angular/material';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  sliders = new Array<SliderData>();

  @ViewChildren(MatSlider) all;
  allSliders = new SliderData(10000, 0, 'All');
  every1Of2NdSlider = new SliderData(10001, 0, '(2/1)');
  every2Of2NdSlider = new SliderData(10001, 0, '(2/2)');

  ngOnInit(): void {
    for (let i = 0; i < 10; ++i) {
      this.sliders.push(new SliderData(i, 0, '#' + (i + 1)));
    }
  }

  onSingleSliderValueChanged(sd: MatSliderChange) {
    const allArray = this.all.toArray();
    allArray[10].value = 0;
    let nr = null;
    this.all.find((item: MatSlider, index: number, array: MatSlider[]) => {
      if (item === sd.source) {
        nr = index;
        return true;
      }
      return false;
    });

    if (nr % 2 === 0) {
      allArray[11].value = 0;
    } else {
      allArray[12].value = 0;
    }
  }

  onAllSlidersChanged(sd: MatSliderChange) {
    const allArray = this.all.toArray();
    allArray[11].value = sd.value;
    allArray[12].value = sd.value;
    for (let i = 0; i < 10; ++i) {
      allArray[i].value = sd.value;
    }
  }

  on1Of2SliderValueChanged(sd: MatSliderChange) {
    const allArray = this.all.toArray();
    allArray[10].value = 0;
    for (let i = 0; i < 10; i += 2) {
      allArray[i].value = sd.value;
    }
  }

  on2Of2SliderValueChanged(sd: MatSliderChange) {
    const allArray = this.all.toArray();
    allArray[10].value = 0;
    for (let i = 1; i < 10; i += 2) {
      allArray[i].value = sd.value;
    }
  }

}
