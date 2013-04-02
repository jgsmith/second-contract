SC.Views.VerbList = Backbone.View.extend
  initialize: ->
    this.model.bind 'reset', this.render, this
    this.model.bind 'add', this.appendNewVerb, this
    this.template = _.template $('#verb-list-template').html()

  render: ->
    this.$el.html this.template {}
    this.$ul = this.$el.find("ul").first()
    for verb in this.model.models
      this.appendNewVerb verb
    this.el

  appendNewVerb: (verb) ->
    this.$ul.append(new SC.Views.VerbListItem({model:verb}).render())
