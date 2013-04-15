class SC.Views.WardList extends Backbone.View
  initialize: (options) ->
    @model.bind 'reset', @render, this
    @model.bind 'add', @appendNewWard, this
    @template = _.template $('#ward-list-template').html()
    @domain = options.domain
    @area = options.area

  render: ->
    @$el.html @template
      domain: @domain
      area: @area
    for area in @model.models
      @appendNewArea area
    @el

  appendNewWard: (ward) ->
    @$el.append(new SC.Views.WardListItem({model:ward,domain:@domain,area:@area}).render())
