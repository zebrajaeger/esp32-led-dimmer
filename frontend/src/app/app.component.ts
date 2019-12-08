import {Component, OnInit} from '@angular/core';
import {SliderData} from './channel-slider/channel-slider.data';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  sliders = new Array<SliderData>();
  allSliders = new SliderData(10000, 0);
  // every2NdSlider = new SliderData(10001, 0);
  // every5NdSlider = new SliderData(10002, 0);

  ngOnInit(): void {
    for (let i = 0; i < 10; ++i) {
      this.sliders.push(new SliderData(i, 0));
    }
  }

  onSingleSliderValueChanged(sd: SliderData) {
    this.allSliders.value = 0;
    this.sliders[sd.channel].value = sd.value;
  }

  onAllSlidersChanged(sd: SliderData) {
    for (const s of this.sliders) {
      s.value = sd.value;
    }
  }
}
