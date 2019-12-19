import {Component, EventEmitter, Input, Output} from '@angular/core';
import {SliderData} from './channel-slider.data';
import {MatSliderChange} from '@angular/material';

@Component({
  selector: 'app-channel-slider',
  templateUrl: './channel-slider.component.html',
  styleUrls: ['./channel-slider.component.scss']
})
export class ChannelSliderComponent {
  @Input() label: string;
  @Input() value: number;
  @Output() valueChanged = new EventEmitter<SliderData>();
  @Input() min = 0;
  @Input() max = 4096;
  @Input() channelNr = 0;

  onChange(e: MatSliderChange) {
    this.valueChanged.emit(new SliderData(this.channelNr, e.value, this.label));
  }
}
